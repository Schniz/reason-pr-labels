type label = {name: string};
type pull_request = {
  labels: list(string),
  statuses_url: string,
};
type pull_request_payload = {pull_request};

type status_state =
  | Success
  | Pending;
let string_of_status =
  fun
  | Success => "success"
  | Pending => "pending";
type status_post_payload = {
  description: string,
  context: string,
  state: status_state,
};

let label_of_json = json => Ezjsonm.(get_string(find(json, ["name"])));

let pull_request_of_json = json =>
  Ezjsonm.{
    labels:
      find(json, ["pull_request", "labels"]) |> get_list(label_of_json),
    statuses_url: find(json, ["pull_request", "statuses_url"]) |> get_string,
  };

let json_of_status_post_payload = spp =>
  Ezjsonm.(
    dict([
      ("state", string_of_status(spp.state) |> string),
      ("description", string(spp.description)),
      ("context", string(spp.context)),
    ])
  );

let successPayload = {
  description: "Alright! PR has valid label(s).",
  context: "pr/labels",
  state: Success,
};
let pendingPayload = {
  description: "Don't forget to add a PR label!",
  context: "pr/labels",
  state: Pending,
};
