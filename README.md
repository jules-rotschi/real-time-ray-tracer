# Real time ray tracer

A real time application that renders ray traced images of scenes with spheres and sky.

## Features

- Opaque material (whith albedo, roughness and shine parameters)
- Clear material (whith albedo, roughness and refractive index parameters)
- Emissive material (whith emitted color parameter)
- Accumulation (several rays for each pixel) when camera is static
- Renderer settings (preview, ray tracing depth, antialiasing)
- Camera moves (translation and rotation)
- Camera realistic settings (exposure, focal length, depth of field)
- Focus peaking (sharpness highlighting)
- Scene settings (materials, spheres positions and radiuses)
- GPU rendering
- Virtual pixel positions caching optimization

## Upcoming features

- Export to PPM file (come back)
- Triangles & complex scenes
- AABB's optimization
- Bloom
- More control over scene from UI

## Buiding

- Clone the repository : `git clone https://github.com/jules-rotschi/real-time-ray-tracer.git`;
- Install Premake if it is not installed on your system;
- In the project directory, run `premake5 <your build tool>`;
- Build with your build tool.