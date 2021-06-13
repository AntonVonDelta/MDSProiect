var frame_data=null;
var ctx=null;

async function renderScene() {
  var input = document.getElementById("render-input").value;
  var scene = new SceneRenderer();

  var canvas = document.getElementById("scene-canvas");
  ctx = canvas.getContext("2d");

  frame_data = await scene.login();
  await scene.loadFromText(input);

  await drawFrames();
}

async function drawFrames(){
  var width=frame_data.width;
  var height=frame_data.height;
  var reader = frame_data.stream.getReader()
  var arr = new Uint8ClampedArray(width * height * 4);

  var loaded=0;
  if(width!=300 || height!=300) throw "Err";

  while (true) {
    const { done, value } = await reader.read();
    if (done) break;

    for (var i = 0; i < value.length; i++) {
      arr[(loaded+i)%arr.length] = value[i];
    }
    loaded+=value.length;
    loaded%=arr.length;

    console.log(`${value.length} bytes downloaded\t${loaded} current bytes for frame\t${arr.length-loaded} bytes left`);

      ctx.putImageData(new ImageData(arr, width,height), 0, 0,0,0,300,300);
      // createImageBitmap(new ImageData(arr, width, height)).then(function(bitmap){
      //     ctx.drawImage(bitmap, 0, 0);
      // });
    
  }

}
