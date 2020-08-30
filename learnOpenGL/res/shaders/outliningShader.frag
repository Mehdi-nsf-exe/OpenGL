#version 330 core

uniform vec4 OutliningColor;

out vec4 FragColor;

void main() {
   FragColor = OutliningColor;
}