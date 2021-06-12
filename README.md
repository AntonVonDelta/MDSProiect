# Remote3D
[![Codacy Badge](https://app.codacy.com/project/badge/Grade/8a32c6d90aae4017be0187b8277b907b)](https://www.codacy.com/gh/AntonVonDelta/MDSProiect/dashboard?utm_source=github.com&amp;utm_medium=referral&amp;utm_content=AntonVonDelta/MDSProiect&amp;utm_campaign=Badge_Grade)
#### Remote visualization and control of 3D renderings by computation offloading - with Web Interface!
This project tackles a major issue for current 3D rendering tehnologies in the browser: limited CPU and GPU resources. Our solution is suited for mobile and low powered devices or embedded graphical systems with limited processing power or for old systems incompatible with the new WegGL technology.

One can easily use our web interface to talk to and receive responses from our custom built http server which processes and renders the requested scenes.
The main ideea of the project is to offload all the GPU computation needed by the client to render an object to another server which will perform all the hard-work. When the requested scene is rendered, the memory buffer is sent in a special format accepted by the http protocol to the client which can visualise the results in their own browser.

The website interface will help the user to use the platorm and will provide a minimal number of demo-objects to be rendered. The user will be able to move the rendered scene using the controls for movement and rotation. All the scenes will be displayed in near real time.

The server will provide an http-compatible api for manipulating the scene. The server will also be able to render custom built objects provided by the client through the web interface.

**Download release version from [here](https://github.com/AntonVonDelta/MDSProiect/releases)**

**Read the protocol [here](https://github.com/AntonVonDelta/MDSProiect/wiki/Server-API)**
## Team
- Achimescu Valentin
- Vilceanu Razvan-Arthur
- Reznicencu Sergiu
- Pirlog Vlad

## Backlog
The backlog is contained here: [Backlog](https://github.com/AntonVonDelta/MDSProiect/projects/)

## Unit Tests/Integration
The code tests for the server components: [Unit Tests](https://github.com/AntonVonDelta/MDSProiect/tree/master/Server/3DServer/CodeTests)

## User Stories
User Stories can be found [here](USER_STORIES.md).

## Bug reports
Issues found here: [#3](https://github.com/AntonVonDelta/MDSProiect/issues/3) and [#15](https://github.com/AntonVonDelta/MDSProiect/issues/15)

## API
The server API for the http protocol is found [here](https://github.com/AntonVonDelta/MDSProiect/wiki/Server-API)

## Arhitectura/UML
![Design](https://github.com/AntonVonDelta/MDSProiect/blob/master/UMLDesign.png)
