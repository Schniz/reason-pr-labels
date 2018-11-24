let string_of_file_path = path => {
  let ic = open_in(path);
  let len = in_channel_length(ic);
  let s = really_input_string(ic, len);
  close_in(ic);
  s;
};

let port =
  switch (Sys.argv[1]) {
  | str => int_of_string(str)
  | exception (Invalid_argument(_)) => 3000
  };

let `RSA(key) =
  string_of_file_path("./github.private-key.pem")
  |> Cstruct.of_string
  |> X509.Encoding.Pem.Private_key.of_pem_cstruct1;

Printf.sprintf("Listening on %d. Ctrl-C to quit!", port) |> print_endline;

Server.make(~port, ~key) |> Lwt_main.run |> ignore;
