<div align="center">

# Sunset

### A Solar Dial inspired watchface for Pebble

[![Pebble App Store](https://img.shields.io/badge/Pebble-App_Store-ff6900?style=for-the-badge&logo=pebble&logoColor=white)](https://apps.repebble.com/42cab49dbb4d4b26bb981ecb)
![Language](https://img.shields.io/badge/Built_with-C-a8b9cc?style=for-the-badge&logo=c&logoColor=white)
![Platforms](https://img.shields.io/badge/Platforms-5-6c5ce7?style=for-the-badge)

*Track the sun across a 24-hour dial — daylight, twilight, and night at a glance.*

</div>

---

<table align="center">
  <tr>
    <td align="center" valign="middle">
      <img src="screenshots/gabbro_1.0_20260418-135124.gif" alt="Gabbro" width="170"/><br/>
      <sub><b>Gabbro</b></sub>
    </td>
    <td align="center" valign="middle">
      <img src="screenshots/emery_1.0_20260418-135207.gif" alt="Emery" width="170"/><br/>
      <sub><b>Emery</b></sub>
    </td>
    <td align="center" valign="middle">
      <img src="screenshots/basalt_1.0_20260418-135151.gif" alt="Basalt" width="170"/><br/>
      <sub><b>Basalt</b></sub>
    </td>
    <td align="center" valign="middle">
      <img src="screenshots/chalk_1.0_20260418-135138.gif" alt="Chalk" width="170"/><br/>
      <sub><b>Chalk</b></sub>
    </td>
    <td align="center" valign="middle">
      <img src="screenshots/flint_1.0_20260418-135221.gif" alt="Flint" width="170"/><br/>
      <sub><b>Flint</b></sub>
    </td>
  </tr>
</table>

---

## About

Built motivated by the [Spring 2026 Pebble App Contest](https://repebble.com/blog/spring-2026-pebble-app-contest) as a proof of concept. Sunset is inspired by the [Solar Dial watchface on Apple Watch](https://support.apple.com/guide/watch/faces-and-features-apde9218b440/watchos#apd348fe40c5) and maps a full day onto a single 24-hour analog face — so you can *see* where you are in the day relative to sunrise and sunset.

> I had no prior Pebble experience, so I used the official Pebble Claude Skill to scaffold the project. It's written in C; a modern JavaScript (ALLOY) rewrite would probably be preferable.

## Features

- **24-hour dial** with a sun/moon indicator that orbits the face throughout the day
- **Colored arcs** distinguish daylight, twilight, and night at a glance
- **Sunrise & sunset times** fetched from [open-meteo.com](https://api.open-meteo.com) — via location services or manual lat/lon
- **Digital time** in 24h format, centered on the face
- **Date** above the digital time
- **Sunrise & sunset times** displayed below the digital time
- Runs on **Gabbro, Emery, Basalt, Chalk, and Flint**

## Install

Grab it from the [Pebble App Store](https://apps.repebble.com/42cab49dbb4d4b26bb981ecb).

## Roadmap

Pull requests are very welcome.

- [ ] Keep the digital time display from drifting outside the center circle
- [ ] Compute sunrise/sunset locally instead of relying on open-meteo
- [ ] App icon and store wallpaper
