let main = () => {
  let () = print_endline("starting");
  Server.start(~ctx=req => (), Schema.schema) |> Lwt_main.run;
};

main();
