let main = () => Server.start(~ctx=req => (), Schema.schema) |> Lwt_main.run;

main();
