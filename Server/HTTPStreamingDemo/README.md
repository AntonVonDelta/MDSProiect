# HTTP Streaming Demo

This demo requires the Node.js runtime and npm. You can download them [here](https://nodejs.org/).

## Install the dependencies

```bash
npm install
```

## Start the app

```bash
npm start
```

An instance of the app will be available at `http://localhost:8000`

When pressing one of the 3 buttons, an image will be streamed from the server and displayed gradually while it's downloading.

In order to slow down the streaming speed, network throttling can be enabled in most modern browsers.
For Google Chrome, this setting is located in `Chrome Dev Tools -> Performance Tab -> Network`
