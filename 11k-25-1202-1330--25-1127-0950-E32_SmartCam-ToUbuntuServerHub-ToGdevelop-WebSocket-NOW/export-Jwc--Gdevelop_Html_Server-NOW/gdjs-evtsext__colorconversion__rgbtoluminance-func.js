
if (typeof gdjs.evtsExt__ColorConversion__RgbToLuminance !== "undefined") {
  gdjs.evtsExt__ColorConversion__RgbToLuminance.registeredGdjsCallbacks.forEach(callback =>
    gdjs._unregisterCallback(callback)
  );
}

gdjs.evtsExt__ColorConversion__RgbToLuminance = {};
gdjs.evtsExt__ColorConversion__RgbToLuminance.idToCallbackMap = new Map();


gdjs.evtsExt__ColorConversion__RgbToLuminance.userFunc0x12375e8 = function GDJSInlineCode(runtimeScene, eventsFunctionContext) {
"use strict";
function toLuminance(color) {
    // Convert the color string to an array of RGB values
    let [r, g, b] = color.split(";");
    let luminance = 0.2126 * r + 0.7152 * g + 0.0722 * b;
    return luminance;
}

const color = eventsFunctionContext.getArgument("color");
eventsFunctionContext.returnValue = toLuminance(color);
};
gdjs.evtsExt__ColorConversion__RgbToLuminance.eventsList0 = function(runtimeScene, eventsFunctionContext) {

{


gdjs.evtsExt__ColorConversion__RgbToLuminance.userFunc0x12375e8(runtimeScene, eventsFunctionContext);

}


};

gdjs.evtsExt__ColorConversion__RgbToLuminance.func = function(runtimeScene, color, parentEventsFunctionContext) {
let scopeInstanceContainer = null;
var eventsFunctionContext = {
  _objectsMap: {
},
  _objectArraysMap: {
},
  _behaviorNamesMap: {
},
  globalVariablesForExtension: runtimeScene.getGame().getVariablesForExtension("ColorConversion"),
  sceneVariablesForExtension: runtimeScene.getScene().getVariablesForExtension("ColorConversion"),
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
if (argName === "color") return color;
    return "";
  },
  getOnceTriggers: function() { return runtimeScene.getOnceTriggers(); }
};


gdjs.evtsExt__ColorConversion__RgbToLuminance.eventsList0(runtimeScene, eventsFunctionContext);


return Number(eventsFunctionContext.returnValue) || 0;
}

gdjs.evtsExt__ColorConversion__RgbToLuminance.registeredGdjsCallbacks = [];