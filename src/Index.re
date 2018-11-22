let port =
  switch (Sys.argv[1]) {
  | str => int_of_string(str)
  | exception (Invalid_argument(_)) => 3000
  };

Printf.sprintf("Listening on %d. Ctrl-C to quit!", port) |> print_endline;

Server.make(~port) |> Lwt_main.run |> ignore;
