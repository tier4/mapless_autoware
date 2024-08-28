# Mapless Architecture for Autoware

## Introduction

This repository provides a prototype implementation of a mapless architecture for Autoware.
The goal of this project is to enable Autoware to work without relying on detailed maps.

## Features

The code of this repository allows to perform planning in Autoware without relying on a HD map.
We assume an external system publishes [`RoadSegments`](./autoware_mapless_planning_msgs/msg/RoadSegments.msg) messages containing a local representation of the driving lanes around the ego vehicle, along with the pose of ego relative to the lanes.
Such system can for example be provided by [driveblocks GmbH](https://www.driveblocks.ai/).

![Architecture](./mapless_architecture/images/mapless_autoware.svg)

## How to use

- Run an external `RoadSegments` provider.
- Run an external odometry signal provider.
  - `/localization/kinematic_state`: pose and velocity of the ego vehicle (in a global frame called `map`, see the notes bellow).
  - `/localization/acceleration`: acceleration of the vehicle.
- Run Autoware without the map and localization components.
- Run this mapless architecture using the [provided launch file](./mapless_architecture/autoware_local_mission_planner/launch/mission_planner_compose.launch.py).

An [example launch file](./mapless.launch.xml) for running Autoware along with the mapless architecture is also provided.

## Repository structure

The repository contains the following sub-folders:

- `autoware_mapless_planning_msgs`: messages used for mapless planning.
- `mapless_architecture`: collection of packages to enable mapless planning.

## About reference frames

Currently, the planning and control modules of Autoware cannot function without receiving the pose of the ego vehicle in `map` frame.
Even if in our case we do not use a map, we must still provide Autoware with an `Odometry` messages (`/localization/kinematic_state`) with the `frame_id` set to `map`.

In the future, Autoware should be adapted to support doing motion planning and control in other frames (e.g., `odom` frame).
This would also allow smooth transitions between using the mapless architecture to using the default map-based architecture by using 2 different reference frames.
