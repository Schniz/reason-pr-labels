type label = {name: string};
type pull_request = {
  labels: list(string),
  statuses_url: string,
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
  };

let sendStatus = (token, pr) => {
  let statusPayload =
    switch (List.find(isValidLabel, pr.labels)) {
    | _ => StatusPayload.success
    | exception Not_found => StatusPayload.pending
    };
  Http.post(
    ~body=StatusPayload.to_json(statusPayload),
    ~auth=Token(token),
    pr.statuses_url
  );
};
