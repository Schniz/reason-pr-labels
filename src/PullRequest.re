type label = {name: string};
type pull_request = {
  labels: list(string),
  statuses_url: string,
  repo_url: string,
};

let startsWith = (substr, str) =>
  Stringext.find_from(~pattern=substr, str) != None;
let isValidLabel = startsWith("PR: ");

let label_of_json = json => Ezjsonm.(get_string(find(json, ["name"])));

let of_json = json =>
  Ezjsonm.{
    labels:
      find(json, ["pull_request", "labels"]) |> get_list(label_of_json),
    statuses_url: find(json, ["pull_request", "statuses_url"]) |> get_string,
    repo_url: find(json, ["repository", "url"]) |> get_string,
  };

let getInstallationToken = (pr, jwt) => {
  let%lwt body = Http.get(~auth=jwt, pr.repo_url ++ "/installation");
  let accessTokenUrl =
    Ezjsonm.(find(from_string(body), ["access_tokens_url"]) |> get_string);
  let%lwt accessTokenResponse = Http.post(~auth=jwt, accessTokenUrl);
  let token =
    Ezjsonm.(
      find(from_string(accessTokenResponse), ["token"]) |> get_string
    );
  Lwt.return(Http.Token(token));
};

let sendStatus = (key, pr) => {
  let statusPayload =
    switch (List.find(isValidLabel, pr.labels)) {
    | _ => StatusPayload.success
    | exception Not_found => StatusPayload.pending
    };
  let%lwt installationToken =
    GithubSign.makeJwt(key) |> getInstallationToken(pr);
  Http.post(
    ~body=StatusPayload.to_json(statusPayload),
    ~auth=installationToken,
    pr.statuses_url,
  );
};
