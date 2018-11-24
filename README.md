# [PRs: Look for Labels](https://github.com/apps/pr-labels-checker)

An experimental GitHub app written in pure ReasonML (or... OCaml!)
It looks up for labels with `PR:` prefix (for autogenerating changelogs for [`yoshi`](https://github.com/wix/yoshi)) and adds a status for the project.

## Under the hood

This project uses [Reason](https://reasonml.github.io) with the native OCaml toolchain to product a statically built binary.
In [`Dockerfile`](./Dockerfile) you can see that we're using plain `alpine` linux image to run the server, resulting in a small image footprint (~16mb).

## Deployment on Heroku

- Create a github app, generate the private token and save it as `github.private-token.pem`
- Deploy with Heroku containers:

  ```bash
  # from the repo root
  heroku login
  heroku create
  heroku container:push web
  heroku container:release web
  heroku open
  ```

## Usage

- Go to https://github.com/apps/pr-labels-checker and install it on your repo
