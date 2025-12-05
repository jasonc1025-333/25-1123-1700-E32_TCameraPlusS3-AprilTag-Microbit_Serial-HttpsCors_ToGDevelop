
if (typeof gdjs.evtsExt__WebSocketClient__onFirstSceneLoaded !== "undefined") {
  gdjs.evtsExt__WebSocketClient__onFirstSceneLoaded.registeredGdjsCallbacks.forEach(callback =>
    gdjs._unregisterCallback(callback)
  );
}

gdjs.evtsExt__WebSocketClient__onFirstSceneLoaded = {};
gdjs.evtsExt__WebSocketClient__onFirstSceneLoaded.idToCallbackMap = new Map();


gdjs.evtsExt__WebSocketClient__onFirstSceneLoaded.userFunc0x93d280 = function GDJSInlineCode(runtimeScene, eventsFunctionContext) {
"use strict";
gdjs.evtTools.wsClient = {
    /** The current connection to the WebSockets server. */
    connection: null,
    /** The last error that occurred. */
    lastError: "",
    /** A list of events triggered. */
    events: [],
};
};
gdjs.evtsExt__WebSocketClient__onFirstSceneLoaded.eventsList0 = function(runtimeScene, eventsFunctionContext) {

{


gdjs.evtsExt__WebSocketClient__onFirstSceneLoaded.userFunc0x93d280(runtimeScene, eventsFunctionContext);

}


};

gdjs.evtsExt__WebSocketClient__onFirstSceneLoaded.func = function(runtimeScene, parentEventsFunctionContext) {
let scopeInstanceContainer = null;
var eventsFunctionContext = {
  _objectsMap: {
},
  _objectArraysMap: {
},
  _behaviorNamesMap: {
},
  globalVariablesForExtension: runtimeScene.getGame().getVariablesForExtension("WebSocketClient"),
  sceneVariablesForExtension: runtimeScene.getScene().getVariablesForExtension("WebSocketClient"),
  localVariables: [],
  getObjects: function(objectName) {
    return eventsFunctionContext._objectArraysMap[objectName] || [];
  },
  getObjectsLists: function(objectName) {
    return eventsFunctionContext._objectsMap[objectName] || null;
  },
  getBehaviorName: function(behaviorName) {
    return eventsFunctionContext._behaviorNamesMap[behaviorName] || behaviorName;
  },
  createObject: function(objectName) {
    const objectsList = eventsFunctionContext._objectsMap[objectName];
    if (objectsList) {
      const object = parentEventsFunctionContext && !(scopeInstanceContainer && scopeInstanceContainer.isObjectRegistered(objectName)) ?
        parentEventsFunctionContext.createObject(objectsList.firstKey()) :
        runtimeScene.createObject(objectsList.firstKey());
      if (object) {
        objectsList.get(objectsList.firstKey()).push(object);
        eventsFunctionContext._objectArraysMap[objectName].push(object);
      }
      return object;
    }
    return null;
  },
  getInstancesCountOnScene: function(objectName) {
    const objectsList = eventsFunctionContext._objectsMap[objectName];
    let count = 0;
    if (objectsList) {
      for(const objectName in objectsList.items)
        count += parentEventsFunctionContext && !(scopeInstanceContainer && scopeInstanceContainer.isObjectRegistered(objectName)) ?
parentEventsFunctionContext.getInstancesCountOnScene(objectName) :
        runtimeScene.getInstancesCountOnScene(objectName);
    }
    return count;
  },
  getLayer: function(layerName) {
    return runtimeScene.getLayer(layerName);
  },
  getArgument: function(argName) {
    return "";
  },
  getOnceTriggers: function() { return runtimeScene.getOnceTriggers(); }
};


gdjs.evtsExt__WebSocketClient__onFirstSceneLoaded.eventsList0(runtimeScene, eventsFunctionContext);


return;
}

gdjs.evtsExt__WebSocketClient__onFirstSceneLoaded.registeredGdjsCallbacks = [];
gdjs.evtsExt__WebSocketClient__onFirstSceneLoaded.registeredGdjsCallbacks.push((runtimeScene) => {
    gdjs.evtsExt__WebSocketClient__onFirstSceneLoaded.func(runtimeScene, runtimeScene);
})
gdjs.registerFirstRuntimeSceneLoadedCallback(gdjs.evtsExt__WebSocketClient__onFirstSceneLoaded.registeredGdjsCallbacks[gdjs.evtsExt__WebSocketClient__onFirstSceneLoaded.registeredGdjsCallbacks.length - 1]);
