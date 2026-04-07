module.exports = [
  {
    "type": "heading",
    "defaultValue": "Solar 24H Settings"
  },
  {
    "type": "text",
    "defaultValue": "Set your location manually, or leave blank to use GPS."
  },
  {
    "type": "section",
    "items": [
      {
        "type": "heading",
        "defaultValue": "Location"
      },
      {
        "type": "input",
        "messageKey": "LAT",
        "label": "Latitude",
        "defaultValue": "",
        "attributes": {
          "placeholder": "e.g. 47.2692",
          "type": "number",
          "step": "any"
        }
      },
      {
        "type": "input",
        "messageKey": "LON",
        "label": "Longitude",
        "defaultValue": "",
        "attributes": {
          "placeholder": "e.g. 11.4041",
          "type": "number",
          "step": "any"
        }
      }
    ]
  },
  {
    "type": "submit",
    "defaultValue": "Save Settings"
  }
];
