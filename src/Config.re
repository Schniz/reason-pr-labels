type t = {
  appId: int,
  webhookSecret: option(string),
  port: int,
};

let parseArguments = () => {
  let port = ref(3000);
  let githubAppId = ref(None);
  let webhookSecret = Sys.getenv_opt("GITHUB_WEBHOOK_SECRET") |> ref;
  let specList = [
    ("--port", Arg.Set_int(port), "Sets the port"),
    (
      "--webhook-secret",
      Arg.String(secret => webhookSecret := Some(secret)),
      "Sets the port",
    ),
    (
      "--app-id",
      Arg.Int(appId => githubAppId := Some(appId)),
      "Sets the GitHub App ID",
    ),
  ];

  Arg.parse(specList, print_endline, "Usage:");

  switch (githubAppId^) {
  | None => failwith("GitHub app id is mandatory.")
  | Some(appId) => {appId, port: port^, webhookSecret: webhookSecret^}
  };
};

let memoized = fn => {
  let _value = fn();
  () => _value;
};

let get = memoized(parseArguments);

let port = c => c.port;
let webhookSecret = c => c.webhookSecret;
let appId = c => c.appId;
