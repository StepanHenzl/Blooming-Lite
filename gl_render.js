var LibraryGLRender = {
	
	//Initializes the WebGL context and sets up shaders, the pixel buffer, and the texture.
	js_init_gl: function(w, h) {
	  const canvas = Module.canvas;
	  const gl = canvas.getContext("webgl");

	  if (!gl) {
		console.error("WebGL not supported");
		return;
	  }

	  canvas.width = w;
	  canvas.height = h;
	  //Make the canvas pixelated when scaled.
	  canvas.style.imageRendering = 'pixelated';

	  gl.viewport(0, 0, w, h);

	  //Vertex shader.
	  const vsSource = `
		attribute vec2 a_position;
		attribute vec2 a_texcoord;
		varying vec2 v_texcoord;
		void main() {
		  gl_Position = vec4(a_position, 0, 1);
		  v_texcoord = a_texcoord;
		}
	  `;

	  //Fragment Shader.
	  const fsSource = `
		precision mediump float;
		varying vec2 v_texcoord;
		uniform sampler2D u_texture;
		uniform vec2 u_texelSize;

		void main() {
		  //Floor to integer to avoid smoothing and add 0.5 to reach center of pixel.
		  vec2 texCoord = (floor(v_texcoord / u_texelSize) + 0.5) * u_texelSize;
		  gl_FragColor = texture2D(u_texture, v_texcoord);
		}
		`;

	  //Compile shaders.
	  const vs = gl.createShader(gl.VERTEX_SHADER);
	  gl.shaderSource(vs, vsSource);
	  gl.compileShader(vs);

	  const fs = gl.createShader(gl.FRAGMENT_SHADER);
	  gl.shaderSource(fs, fsSource);
	  gl.compileShader(fs);

	  //Link shaders into a program.
	  const program = gl.createProgram();
	  gl.attachShader(program, vs);
	  gl.attachShader(program, fs);
	  gl.linkProgram(program);
	  //Make program active.
	  gl.useProgram(program);

	  //Flipped horizontally and vertically.
	  const vertices = new Float32Array([
		-1, -1,  0, 1,
		 1, -1,  1, 1,
		-1,  1,  0, 0,
		 1,  1,  1, 0,
	  ]);
	  
	  //Buffer containing pixel values for the texture.
	  const buffer = gl.createBuffer();
	  gl.bindBuffer(gl.ARRAY_BUFFER, buffer);
	  //Set to static because vertices do not change at runtime.
	  gl.bufferData(gl.ARRAY_BUFFER, vertices, gl.STATIC_DRAW);
	  
	  //Link buffer to shader.
	  const a_position = gl.getAttribLocation(program, "a_position");
	  const a_texcoord = gl.getAttribLocation(program, "a_texcoord");

	  gl.enableVertexAttribArray(a_position);
	  gl.vertexAttribPointer(a_position, 2, gl.FLOAT, false, 16, 0);

	  gl.enableVertexAttribArray(a_texcoord);
	  gl.vertexAttribPointer(a_texcoord, 2, gl.FLOAT, false, 16, 8);

	  const texture = gl.createTexture();
	  gl.bindTexture(gl.TEXTURE_2D, texture);
	  gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MIN_FILTER, gl.NEAREST);
	  gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MAG_FILTER, gl.NEAREST);
	  gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_WRAP_S, gl.CLAMP_TO_EDGE);
	  gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_WRAP_T, gl.CLAMP_TO_EDGE);

	  Module._gl = gl;
	  Module._glTexture = texture;
	  Module._glSize = { w, h };
	},
	
	//Write the distance of the canvas from left top corner, used to calculate start coordinates of canvas.
	js_get_canvas_distance: function(ptrX, ptrY) {
		var rect = document.getElementById('canvas').getBoundingClientRect();
		setValue(ptrX, rect.left, 'double');
		setValue(ptrY, rect.top, 'double');
    },
	
	//Writes the actual screen size for WebGL ratio calculations.
	js_get_canvas_size: function(widthPtr, heightPtr) {
		const rect = Module.canvas.getBoundingClientRect();
		setValue(widthPtr, rect.width, 'double');
		setValue(heightPtr, rect.height, 'double');
  	},

	//Gets pixel data from the C++ array, uploads it to the texture, and draws the frame.
	js_render_frame: function() {
	  const gl = Module._gl;
	  const texture = Module._glTexture;
	  const ptr = Module._get_pixel_buffer?.();
	  const size = Module._glSize;
	  if (!gl || !ptr) return;
	  
	  const w = size.w;
	  const h = size.h;
	  
	  const pixels = new Uint8Array(Module.HEAPU8.buffer, ptr, w * h * 4);

	  gl.bindTexture(gl.TEXTURE_2D, texture);
	  gl.texImage2D(gl.TEXTURE_2D, 0, gl.RGBA, w, h, 0, gl.RGBA, gl.UNSIGNED_BYTE, pixels);

	  gl.clear(gl.COLOR_BUFFER_BIT);
	  gl.drawArrays(gl.TRIANGLE_STRIP, 0, 4);
	}
};

mergeInto(LibraryManager.library, LibraryGLRender);