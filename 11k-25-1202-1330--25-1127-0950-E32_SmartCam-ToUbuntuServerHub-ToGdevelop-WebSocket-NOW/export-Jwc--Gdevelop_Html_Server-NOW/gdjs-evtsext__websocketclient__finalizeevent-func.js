
if (typeof gdjs.evtsExt__WebSocketClient__FinalizeEvent !== "undefined") {
  gdjs.evtsExt__WebSocketClient__FinalizeEvent.registeredGdjsCallbacks.forEach(callback =>
    gdjs._unregisterCallback(callback)
  );
}

gdjs.evtsExt__WebSocketClient__FinalizeEvent = {};
gdjs.evtsExt__WebSocketClient__FinalizeEvent.idToCallbackMap = new Map();


gdjs.evtsExt__WebSocketClient__FinalizeEvent.userFunc0x1387da8 = function GDJSInlineCode(runtimeScene, eventsFunctionContext) {
"use strict";
gdjs.evtTools.wsClient.events.pop();

};
gdjs.evtsExt__WebSocketClient__FinalizeEvent.eventsList0 = function(runtimeScene, eventsFunctionContext) {

{


gdjs.evtsExt__WebSocketClient__FinalizeEvent.userFunc0x1387da8(runtimeScene, eventsFunctionContext);

}


};

gdjs.evtsExt__WebSocketClient__FinalizeEvent.func = function(runtimeScene, parentEventsFunctionContext) {
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


gdjs.evtsExt__WebSocketClient__FinalizeEvent.eventsList0(runtimeScene, eventsFunctionContext);


return;
}

gdjs.evtsExt__WebSocketClient__FinalizeEvent.registeredGdjsCallbacks = [];