# Remote3D
#### Remote visualization and control of 3D renderings by computation offloading - with Web Interface!
This project tackles a major issue for current 3D rendering tehnologies in the browser: limited CPU and GPU resources. 
One can easily use our web interface to talk to and receive responses from our custom built http server which processes and renders the requested scenes.
The main ideea of the project os to offload all the GPU computation needed by the client to render an object to another server which will perform all the hard-work. When the requested scene is rendered the memory buffer is sent in a special format accepted by the http protocol to the client which can visualise the result in their own browser.

The website interface will help the user to use the platorm and will provide a minimal number of demo-objects to be rendered. The user will be able to move the rendered scene using the controls for movement and rotation. All the scenes will be displayed in near real time.

## Team
- Achimescu Valentin
- Vilceanu Razvan-Arthur
- Reznicencu Sergiu

## Backlog
The backlog is contained here: https://github.com/AntonVonDelta/MDSProiect/projects/

## Unit Tests/Integration
Now comes with Integration!

## User Stories
User Stories can be found here: https://github.com/AntonVonDelta/MDSProiect/blob/master/User%20Stories.txt

## Arhitectura/UML
