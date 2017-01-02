module.exports = [
  {
    "type": "heading",
    "defaultValue": "App Configuration"
  },
  {
    "type": "text",
    "defaultValue": "Here is some introductory text."
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
      }
    ]
  },
  {
    "type": "submit",
    "defaultValue": "Save Settings"
  }
];