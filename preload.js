Module.onRuntimeInitialized = function() {
  Module.ccall('start_game', null, [], []);
};