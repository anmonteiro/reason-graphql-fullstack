open Lwt;

module C = Cohttp_lwt_unix;

module CServer = Cohttp_lwt_unix.Server;

module Schema = Graphql_lwt.Schema;

let serveStatic = (base, path) => {
  let fname = Filename.concat(base, path);
  if (Sys.file_exists(fname)) {
    if (Sys.is_directory(fname)) {
      if (Sys.file_exists(Filename.concat(fname, "index.html"))) {
        CServer.respond_file(
          ~fname=Filename.concat(fname, "index.html"),
          (),
        );
      } else {
        C.Server.respond_string(~status=`Not_found, ~body="", ());
      };
    } else {
      CServer.respond_file(~fname, ());
    };
  } else if (Sys.file_exists(fname ++ ".html")) {
    CServer.respond_file(~fname=fname ++ ".html", ());
  } else {
    C.Server.respond_string(~status=`Not_found, ~body="", ());
  };
};

let json_err =
  fun
  | Ok(_) as ok => ok
  | Error(err) => Error(`String(err));

let execute_query = (ctx, schema, variables, query) =>
  Lwt_result.(
    Lwt.return @@
    json_err @@
    Graphql_parser.parse(query)
    >>= (doc => Schema.execute(schema, ctx, ~variables, doc))
  );

let execute_request = (ctx, schema, req, body) =>
  Cohttp_lwt.Body.to_string(body)
  >>= (
    body' => {
      Lwt_io.printf("Body: %s\n", body');
      let json = Yojson.Basic.from_string(body');
      let query =
        Yojson.Basic.(json |> Util.member("query") |> Util.to_string);
      let variables =
        try (Yojson.Basic.Util.(json |> member("variables") |> to_assoc)) {
        | _ => []
        };
      Lwt_io.printf("Query: %s\n", query);
      let result =
        execute_query(
          ctx,
          schema,
          (variables :> list((string, Graphql_parser.const_value))),
          query,
        );
      result
      >>= (
        fun
        | Ok(data) => {
            let body = Yojson.Basic.to_string(data);
            C.Server.respond_string(~status=`OK, ~body, ());
          }
        | Error(err) => {
            let body = Yojson.Basic.to_string(err);
            C.Server.respond_error(~body, ());
          }
      );
    }
  );

let mk_callback = (mk_context, schema, conn, req: Cohttp.Request.t, body) => {
  Lwt_io.printf("Req: %s\n", req.resource);
  let req_path = Cohttp.Request.uri(req) |> Uri.path;
  let path_parts = Str.(split(regexp("/"), req_path));
  switch (req.meth, path_parts) {
  | (`GET, ["graphql"]) => serveStatic("./public", "graphiql.html")
  /* | (`GET, ["graphql", path]) => serveStatic(path) */
  | (`POST, ["graphql"]) =>
    execute_request(mk_context(req), schema, req, body)
  | (`GET, _) => serveStatic("./public", req_path)
  | _ => C.Server.respond_string(~status=`Not_found, ~body="", ())
  };
};

let start = (~port=8080, ~ctx, schema) => {
  let callback = mk_callback(ctx, schema);
  C.Server.create(~mode=`TCP(`Port(port)), C.Server.make(~callback, ()));
};
