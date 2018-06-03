module type Config = {
  type data;
  type state;
  type action;
  let query: {
    .
    "parse": Js.Json.t => data,
    "query": string,
    "variables": Js.Json.t,
  };
  let make:
    (~data: data, array(int)) =>
    ReasonReact.componentSpec(
      state,
      state,
      ReasonReact.noRetainedProps,
      ReasonReact.noRetainedProps,
      action,
    );
};

let style = ReactDOMRe.Style.make;

exception Graphql_error(string);

let sendQuery = q =>
  Bs_fetch.(
    fetchWithInit(
      "/graphql",
      RequestInit.make(
        ~method_=Post,
        ~body=
          Js.Dict.fromList([
            ("query", Js.Json.string(q##query)),
            ("variables", q##variables),
          ])
          |> Js.Json.object_
          |> Js.Json.stringify
          |> BodyInit.make,
        ~credentials=Include,
        ~headers=
          HeadersInit.makeWithArray([|
            ("content-type", "application/json"),
          |]),
        (),
      ),
    )
    |> Js.Promise.then_(resp =>
         if (Response.ok(resp)) {
           Response.json(resp)
           |> Js.Promise.then_(data =>
                switch (Js.Json.decodeObject(data)) {
                | Some(obj) =>
                  Js.Dict.unsafeGet(obj, "data")
                  |> q##parse
                  |> Js.Promise.resolve
                | None =>
                  Js.Promise.reject(
                    Graphql_error("Response is not an object"),
                  )
                }
              );
         } else {
           Response.text(resp)
           |> Js.Promise.then_(text =>
                Js.Promise.reject(
                  Graphql_error(
                    "Request failed: "
                    ++ string_of_int(Response.status(resp))
                    ++ " "
                    ++ Response.statusText(resp)
                    ++ "\n"
                    ++ text,
                  ),
                )
              );
         }
       )
  );

module F = (Config: Config) => {
  type state =
    | Loading
    | Error(string)
    | Loaded(Config.data);
  let component = ReasonReact.reducerComponent("Loader");
  let make = _children => {
    ...component,
    initialState: () => Loading,
    reducer: (action, state) => ReasonReact.Update(action(state)),
    didMount: ({send}) =>
      sendQuery(Config.query)
      |> Js.Promise.then_(response =>
           send(_ => Loaded(response)) |> Js.Promise.resolve
         )
      |> Js.Promise.catch(_ =>
           send(_ => Error("Couldn't get todos")) |> Js.Promise.resolve
         )
      |> ignore,
    render: self =>
      switch (self.state) {
      | Loading =>
        <div
          style=(
            style(~alignItems="center", ~padding="5px", ~color="#ddd", ())
          )>
          (ReasonReact.string("Loading"))
        </div>
      | Error(text) =>
        <div> (ReasonReact.string("Error")) (ReasonReact.string(text)) </div>
      | Loaded(data) => <Config data />
      },
  };
};
