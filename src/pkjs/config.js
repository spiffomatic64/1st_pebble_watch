module.exports = [
  {
    "type": "heading",
    "defaultValue": "App Configuration"
  },
  {
    "type": "text",
    "defaultValue": "TEST!"
  },
  {
    "type": "section",
    "items": [
      {
        "type": "heading",
        "defaultValue": "Features"
      },
      {
        "type": "toggle",
        "messageKey": "Bluetooth",
        "label": "Enable Bluetooth Status",
        "defaultValue": true
      },
      {
        "type": "toggle",
        "messageKey": "Animations",
        "label": "Enable Animations",
        "defaultValue": false
      },
      {
      "type": "input",
      "messageKey": "forecastio",
      "label": "DarkSky Api Key"
      }
    ]
  },
  {
    "type": "submit",
    "defaultValue": "Save Settings"
  }
];