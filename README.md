# [PRs: Look for Labels](https://github.com/Schniz/reason-pr-labels)

Reason-native server that accepts GitHub PR webhooks and sends statuses to verify correct PR labels

## Under the hood

This project uses [Reason](https://reasonml.github.io) with the native OCaml toolchain to product a statically built binary.
In [`Dockerfile`](./Dockerfile) you can see that we're using plain `alpine` linux image to run the server, resulting in a small image footprint (~16mb).

## Deployment on Heroku

Just use Heroku with containers:

```bash
# from the repo root
heroku login
heroku create
heroku container:push web
heroku container:release web
heroku open
```

## Usage

* Deploy to Heroku
* Create labels with `PR: ` prefix :smile_cat:
* Go to GitHub's project settings
* Click on "Webhooks"
* Click on "Add webhook"
* Fill in `Payload URL` `HEROKU_DEPLOYMENT_URL/handle_pull_request?token=YOUR_GITHUB_TOKEN`.
* Select `application/json` in `Content type`
* Select `Let me select individual events`
* On the list, uncheck `Pushes` and check `Pull requests`
* Click `Add webhook`
* :moneybag:
