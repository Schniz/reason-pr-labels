type label = {name: string};
type pull_request = {
  labels: list(string),
  statuses_url: string,
};

module StatusPayload = {
  type state =
    | Pending
    | Success;
  type t = {
    description: string,
    context: string,
    state,
  };
  let string_of_state =
    fun
    | Success => "success"
    | Pending => "pending";
  let to_json = spp =>
    Ezjsonm.(
      dict([
        ("state", string_of_state(spp.state) |> string),
        ("description", string(spp.description)),
        ("context", string(spp.context)),
      ])
    );

  let success = {
    description: "Alright! PR has valid label(s).",
    context: "pr/labels",
    state: Success,
  };
  let pending = {
    description: "Don't forget to add a PR label!",
    context: "pr/labels",
    state: Pending,
  };
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
    Printf.sprintf("%s?access_token=%s", pr.statuses_url, token),
  );
};
