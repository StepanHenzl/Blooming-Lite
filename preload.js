Module.preRun = Module.preRun || [];
Module.preRun.push(function() {
  console.log("Custom preRun hook");
});

Module.onRuntimeInitialized = function() {
  console.log("Runtime initialized");
  Module.ccall('start_game', null, [], []);
};