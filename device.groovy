/**
 *  Globe Lamp
 *
 *  Copyright 2014 davidebliss@gmail.com
 *
 *  Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except
 *  in compliance with the License. You may obtain a copy of the License at:
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software distributed under the License is distributed
 *  on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License
 *  for the specific language governing permissions and limitations under the License.
 *
 */
metadata {
	definition (name: "Globe Lamp", author: "davidebliss@gmail.com") {
		capability "Switch"
		capability "Switch Level"
		capability "Color Control"
		capability "Actuator"

		attribute "position", "string"
		attribute "bothbutton", "string"
		attribute "topbutton", "string"
		attribute "bottombutton", "string"
		attribute "rananimatebutton", "string"
		attribute "animation1button", "string"
		attribute "animation2button", "string"
		attribute "animation3button", "string"
		attribute "animation4button", "string"
		attribute "animation5button", "string"
		attribute "color1button", "string"
		attribute "color2button", "string"
		attribute "color3button", "string"
		attribute "color4button", "string"
		attribute "color5button", "string"
		attribute "color6button", "string"

		command "setPosition"
		command "both"
		command "top"
		command "bottom"
		command "animate"
		command "animateOne"
		command "animateTwo"
		command "animateThree"
		command "animateFour"
		command "animateFive"
		command "setAdjustedColor"
		command "setColorOne"
		command "setColorTwo"
		command "setColorThree"
		command "setColorFour"
		command "setColorFive"
		command "setColorSix"
	}

	simulator {
		// TODO: define status and reply messages here
	}

	tiles {
        
		standardTile("switch", "device.switch", width: 1, height: 1, canChangeIcon: true, canChangeBackground: true) {
			state "on", label: '${name}', action: "switch.off", backgroundColor: "#79b821"
			state "off", label: '${name}', action: "switch.on", backgroundColor: "#ffffff"
		}
        standardTile("bothbutton", "bothbutton", width: 1, height: 1) {
			state "1", label: 'both', action: "both", backgroundColor: "#ffffff"
		}
        standardTile("topbutton", "topbutton", width: 1, height: 1) {
			state "1", label: 'top', action: "top", backgroundColor: "#ffffff"
		}
        standardTile("bottombutton", "bottombutton", width: 1, height: 1) {
			state "2", label: 'bottom', action: "bottom", backgroundColor: "#ffffff"
		}
        standardTile("rananimatebutton", "rananimatebutton", width: 1, height: 1) {
			state "animate", label: 'random', action: "animate", backgroundColor: "#ffffff"
		}
        standardTile("animation1button", "animation1button", width: 1, height: 1) {
			state "animate", label: 'one', action: "animateOne", backgroundColor: "#ffffff"
		}
        standardTile("animation2button", "animation2button", width: 1, height: 1) {
			state "animate", label: 'two', action: "animateTwo", backgroundColor: "#ffffff"
		}
        standardTile("animation3button", "animation3button", width: 1, height: 1) {
			state "animate", label: 'three', action: "animateThree", backgroundColor: "#ffffff"
		}
        standardTile("animation4button", "animation4button", width: 1, height: 1) {
			state "animate", label: 'four', action: "animateFour", backgroundColor: "#ffffff"
		}
        standardTile("animation5button", "animation5button", width: 1, height: 1) {
			state "animate", label: 'five', action: "animateFive", backgroundColor: "#ffffff"
		}
        controlTile("level", "device.level", "slider", height: 1, width: 2, inactiveLabel: false) {
			state "level", action:"switch level.setLevel"
		}
        controlTile("position", "position", "slider", height: 1, width: 3, inactiveLabel: false) {
			state "level", label: 'position', action:"setPosition"
		}
        
        standardTile("color1button", "color1button", height: 1, width: 1) {
			state "setColorOne", action: "setColorOne", backgroundColor: "#ffffff"
        }
        
        standardTile("color2button", "color2button", height: 1, width: 1) {
			state "setColorTwo", action: "setColorTwo", backgroundColor: "#ffff00"
        }
        
        standardTile("color3button", "color3button", height: 1, width: 1) {
			state "setColorThree", action: "setColorThree", backgroundColor: "#ff9900"
        }
        
        standardTile("color4button", "color4button", height: 1, width: 1) {
			state "setColorFour", action: "setColorFour", backgroundColor: "#ff0000"
        }
        
        standardTile("color5button", "color5button", height: 1, width: 1) {
			state "setColorFive", action: "setColorFive", backgroundColor: "#00ff00"
        }
        
        standardTile("color6button", "color6button", height: 1, width: 1) {
			state "setColorSix", action: "setColorSix", backgroundColor: "#0000ff"
        }
        
        
        controlTile("rgbSelector", "device.color", "color", height: 3, width: 3, inactiveLabel: false) {
			state "color", action:"setAdjustedColor"
        }
        
        main "switch"
		details(["switch", "level", "bothbutton", "topbutton", "bottombutton", "position", "rananimatebutton", "animation1button", "animation2button", "animation3button", "animation4button", "animation5button", "color1button", "color2button","color3button","color4button","color5button","color6button","rgbSelector"])
	}
}

// parse events into attributes
def parse(String description) {
	log.debug "Parsing '${description}'"
	def value = zigbee.parse(description)?.text
	def name = value && value != "ping" ? "response" : null
    
    log.debug "zigbee.parse value: '${value}'"
    
    if (value.startsWith('on')) {
    	name = "switch"
    	log.debug "switch detected: '${value}'"
    } else if (value.startsWith('off')) {
    	name = "switch"
    	log.debug "switch detected: '${value}'"
    } else if (value.startsWith('setLevel:')){
    	value -= "setLevel:"
        value = value.trim()
    	log.debug "setLevel detected: '${value}'"
        name = "level"
    } else if (value.startsWith('setPosition:')){
    	value -= "setPosition:"
        value = value.trim()
    	log.debug "setPosition detected: '${value}'"
        name = "position"
    } else if (value.startsWith('sc:')){
    	value -= "sc:"
        value = value.trim()
    	log.debug "sc detected: '${value}'"
        name = "rgbSelector"
    } else if (value.startsWith('animate')){
        name = "animate"
    	log.debug "animate detected"
    } else if (value.startsWith('top')){
        name = "top"
    	log.debug "top detected"
    } else if (value.startsWith('bottom')){
        name = "bottom"
    	log.debug "bottom detected"
    } else if (value.startsWith('both')){
        name = "both"
    	log.debug "both detected"
    } 
    
    def result = createEvent(name: name, value: value)
	log.debug "Parse returned ${result?.descriptionText}"
	return result	
}

// handle commands
def on() {
	log.debug "Executing 'on'"
    zigbee.smartShield(text: "o: 1").format()
}

def off() {
	log.debug "Executing 'off'"
	zigbee.smartShield(text: "o: 0").format()
}

def setLevel(level) {
	log.debug "Executing 'setLevel: ${level}'"
	zigbee.smartShield(text: "sl:$level").format()
}

def setColorOne() {
	log.debug "Executing color button 1"
	zigbee.smartShield(text: "sc:255,150,150").format()
}

def setColorTwo() {
	log.debug "Executing color button 2"
	zigbee.smartShield(text: "sc:255,255,0").format()
}
def setColorThree() {
	log.debug "Executing color button 3"
	zigbee.smartShield(text: "sc:255,150,0").format()
}

def setColorFour() {
	log.debug "Executing color button 4"
	zigbee.smartShield(text: "sc:255,0,0").format()
}
def setColorFive() {
	log.debug "Executing color button 5"
	zigbee.smartShield(text: "sc:0,255,0").format()
}
def setColorSix() {
	log.debug "Executing color button 6"
	zigbee.smartShield(text: "sc:0,0,255").format()
}
def setAdjustedColor(value) {
	log.debug "setAdjustedColor: ${value}"
	def adjusted = value + [:]
	setColor(adjusted)
}

def adjustOutgoingHue(percent) {
	def adjusted = percent
	if (percent > 31) {
		if (percent < 63.0) {
			adjusted = percent + (7 * (percent -30 ) / 32)
		}
		else if (percent < 73.0) {
			adjusted = 69 + (5 * (percent - 62) / 10)
		}
		else {
			adjusted = percent + (2 * (100 - percent) / 28)
		}
	}
	log.info "percent: $percent, adjusted: $adjusted"
	adjusted
}

def setColor(value) {
	log.debug "sc:$value.red,$value.green,$value.blue"
	zigbee.smartShield(text: "sc:$value.red,$value.green,$value.blue").format()
}

def setPosition(pos) {
	log.debug "Executing 'setPosition: ${pos}'"
	zigbee.smartShield(text: "sp: $pos").format()
}

def animate() {
	log.debug "Executing 'animate'"
	zigbee.smartShield(text: "a: r").format()
}

def animateOne() {
	log.debug "Executing 'animateOne'"
	zigbee.smartShield(text: "a: 1").format()
}

def animateTwo() {
	log.debug "Executing 'animateTwo'"
	zigbee.smartShield(text: "a: 2").format()
}
def animateThree() {
	log.debug "Executing 'animateThree'"
	zigbee.smartShield(text: "a: 3").format()
}
def animateFour() {
	log.debug "Executing 'animateFour'"
	zigbee.smartShield(text: "a: 4").format()
}
def animateFive() {
	log.debug "Executing 'animateFive'"
	zigbee.smartShield(text: "a: 5").format()
}
def both() {
	log.debug "Executing 'both'"
	zigbee.smartShield(text: "m: 0").format()
}
def top() {
	log.debug "Executing 'top'"
	zigbee.smartShield(text: "m: 1").format()
}
def bottom() {
	log.debug "Executing 'bottom'"
	zigbee.smartShield(text: "m: 2").format()
}
