let main = () => Server.start(~ctx=_req => (), Schema.schema) |> Lwt_main.run;

main();
