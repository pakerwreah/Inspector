#!/usr/bin/env bash
./gradlew clean
./gradlew assembleRelease
./gradlew artifactoryPublish