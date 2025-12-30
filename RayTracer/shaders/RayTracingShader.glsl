#version 460 core

struct RendererSettings
{
	bool Preview;
	uint PreviewMode;
	bool Accumulation;
	bool Antialiasing;
	bool DepthOfField;
	bool FocusPeaking;
	uint Depth;
};

struct Camera
{
	vec4 Position;
	vec4 E1;
	vec4 E2;
	vec4 E3;
	float SensorWidth;
	float SensorHeight;
	uint Sensitivity;
	float Gamma;
	float FocalLength;
	float FocusDistance;
	float Aperture;
	float ShutterSpeed;
	float NeutralDensityValue;
};

struct Material
{
	bool IsOpaque;
	vec4 Albedo;
	vec4 EmittedColor;
	float Roughness;
	float Shine;
	float RefractiveIndex;
};

struct Sphere
{
	vec4 Position;
	float Radius;
	uint MaterialIndex;
};

struct Interval
{
	float Minimum;
	float Maximum;
};

struct Ray
{
	vec3 Origin;
	vec3 Direction;
	float CurrentMediumRefractiveIndex;
};

struct HitPayload
{
	float T;
	vec3 Position;
	vec3 UnitNormal;
	bool FrontFace;
	uint MaterialIndex;
};

layout(rgba32f, binding = 0) uniform writeonly image2D outputImage;

layout (std140, binding = 1) uniform data
{
    RendererSettings settings;
	uint frameIndex;

	Camera camera;

	vec4 skyColor;
	Material materials[10];
	uint materialsCount;
	Sphere spheres[10];
	uint spheresCount;
};

layout(std430, binding = 2) buffer integratedLuminanceBuffer
{
	vec4 integratedLuminance[];
};

layout(local_size_x = 16, local_size_y = 16) in;

const float PI = 3.141592653589793f;

uint PCG_Hash(uint inputNumber)
{
	uint state = inputNumber * 747796405u + 2891336453u;
	uint word = ((state >> ((state >> 28u) + 4u)) ^ state) * 277803737u;
	return (word >> 22u) ^ word;
}

float RandomFloat(inout uint seed)
{
	seed = PCG_Hash(seed);
	return float(seed) / float(uint(0xffffffff));
}

float RandomFloat(inout uint seed, float minimum, float maximum)
{
	return minimum + (maximum - minimum) * RandomFloat(seed);
}

vec3 RandomVector(inout uint seed)
{
	return vec3(RandomFloat(seed), RandomFloat(seed), RandomFloat(seed));
}

vec3 RandomVector(inout uint seed, float minimum, float maximum)
{
	return vec3(RandomFloat(seed, minimum, maximum), RandomFloat(seed, minimum, maximum), RandomFloat(seed, minimum, maximum));
}

vec3 RandomVectorInUnitDisk(inout uint seed)
{
	float angle = RandomFloat(seed, 0.0, 2.0 * PI);
	float radius = sqrt(RandomFloat(seed));
	return normalize(vec3(radius * cos(angle), radius * sin(angle), 0.0));
}

vec3 RandomVectorInUnitSphere(inout uint seed)
{
	float z = RandomFloat(seed, -1.0, 1.0);
	float radius = sqrt(1.0 - z * z);
	float longitude = RandomFloat(seed, -PI, PI);
	return normalize(vec3(radius * cos(longitude), radius * sin(longitude), z) * pow(RandomFloat(seed), 1/3));
}

bool DoesIntervalContain(float x, Interval i)
{
	return x >= i.Minimum && x <= i.Maximum;
}

void SetFaceNormal(inout HitPayload hit, Ray ray, vec3 outwardNormal)
{
	hit.FrontFace = dot(ray.Direction, outwardNormal) < 0;
	hit.UnitNormal = faceforward(outwardNormal, ray.Direction, outwardNormal);
}

float GetExposure()
{
	return camera.Sensitivity * camera.ShutterSpeed / (270 * camera.Aperture * camera.Aperture * pow(10, camera.NeutralDensityValue));
}

vec4 GetPixel(vec3 luminance)
{
	vec3 linearValue = luminance * PI * GetExposure() / 0.89;
	linearValue.r = linearValue.r > 1 ? 1 : linearValue.r;
	linearValue.g = linearValue.g > 1 ? 1 : linearValue.g;
	linearValue.b = linearValue.b > 1 ? 1 : linearValue.b;

	vec3 gammaCorrectedValue;
	gammaCorrectedValue.r = pow(linearValue.r, camera.Gamma);
	gammaCorrectedValue.g = pow(linearValue.g, camera.Gamma);
	gammaCorrectedValue.b = pow(linearValue.b, camera.Gamma);

	return vec4(gammaCorrectedValue, 1.0);
}

vec3 GetRayOrigin(inout uint seed)
{
	if (!settings.DepthOfField)
		return vec3(camera.Position);

	float apertureRadius = (camera.FocalLength / camera.Aperture) / 2;
	return vec3(camera.Position) + RandomVectorInUnitDisk(seed) * apertureRadius;
}

vec3 GetVirtualPixelPosition(uint x, uint y, out float pixelDimension)
{
	float sensorRatio = camera.SensorWidth / camera.SensorHeight;
	float imageRatio = float(imageSize(outputImage).x) / float(imageSize(outputImage).y);

	float sensorActiveWidth = camera.SensorWidth;
	float sensorActiveHeight = camera.SensorHeight;

	if (sensorRatio < imageRatio)
		sensorActiveHeight = camera.SensorWidth / imageRatio;

	else if (sensorRatio > imageRatio)
		sensorActiveWidth = camera.SensorHeight * imageRatio;

	float viewportDimensionRatio = camera.FocusDistance / camera.FocalLength;
	float viewportWidth = viewportDimensionRatio * sensorActiveWidth;
	float viewportHeight = viewportDimensionRatio * sensorActiveHeight;

	pixelDimension = viewportWidth / imageSize(outputImage).x;

	vec3 viewportBegin =
		vec3(camera.Position) - (viewportWidth / 2) * vec3(camera.E1)
		+ (viewportHeight / 2) * vec3(camera.E2)
		- camera.FocusDistance * vec3(camera.E3);

	vec3 pixelBegin =
		viewportBegin + x * pixelDimension * vec3(camera.E1) - y * pixelDimension * vec3(camera.E2);

	vec3 pixelCenter =
		pixelBegin + (pixelDimension / 2) * vec3(camera.E1 - camera.E2);

	return pixelCenter;
}

vec3 GetVirtualPixelPosition(uint x, uint y)
{
	float _ = 0.0;
	return GetVirtualPixelPosition(x, y, _);
}

vec3 GetVirtualPixelPosition(uint x, uint y, inout uint seed)
{
	float pixelDimension = 0.0;
	vec3 pixelCenter = GetVirtualPixelPosition(x, y, pixelDimension);

	if (!settings.Antialiasing)
		return pixelCenter;

	vec3 offset = vec3(0.5 * RandomFloat(seed), 0.5 * RandomFloat(seed), 0);
	return pixelCenter + offset * pixelDimension;
}

float GetVisibleSurface(Sphere sphere)
{
	return PI * sphere.Radius * sphere.Radius;
}

bool ScatterRay(Ray incidentRay, HitPayload hit, inout Ray scatteredRay, inout uint seed)
{
	Material material = materials[hit.MaterialIndex];

	vec3 scatteredDirection = vec3(0.0);

	if (!material.IsOpaque)
	{
		float firstMediumIndex = incidentRay.CurrentMediumRefractiveIndex;
		float secondMediumIndex = hit.FrontFace ? material.RefractiveIndex : 1;
		float refractiveIndicesRatio = firstMediumIndex / secondMediumIndex;

		vec3 incidentUnitVector = normalize(incidentRay.Direction);

		float cosIncidentAngle = dot(incidentUnitVector, -hit.UnitNormal);
		
		bool doesRayOnlyReflect = refractiveIndicesRatio * sqrt(1 - cosIncidentAngle * cosIncidentAngle) > 1;

		float reflectance = (1 - secondMediumIndex) / (1 + secondMediumIndex);
		float squaredReflectance = reflectance * reflectance;
		float reflexionProbability =
			squaredReflectance
			+ (1 - squaredReflectance)
			* pow(1 - cosIncidentAngle, 5);

		if (doesRayOnlyReflect || RandomFloat(seed) < reflexionProbability)
		{
			vec3 reflectedDirection = reflect(incidentUnitVector, hit.UnitNormal);
			scatteredDirection = reflectedDirection + material.Roughness * RandomVectorInUnitSphere(seed);
		}
		else {
			vec3 refractedDirection = refract(incidentUnitVector, hit.UnitNormal, refractiveIndicesRatio);
			scatteredDirection = refractedDirection + material.Roughness * RandomVectorInUnitSphere(seed);
		}

		scatteredRay = Ray(hit.Position, scatteredDirection, secondMediumIndex);

		return true;
	}
	else
	{
		if (material.Shine > 0 && (material.Shine >= 1 || RandomFloat(seed) < material.Shine))
		{
			vec3 reflectedDirection = reflect(incidentRay.Direction, hit.UnitNormal);
			scatteredDirection = normalize(reflectedDirection) + material.Roughness * RandomVectorInUnitSphere(seed);
			if (dot(scatteredDirection, scatteredDirection) < 1e-3)
			{
				scatteredDirection = reflectedDirection;
			}
		}
		else
		{
			scatteredDirection = hit.UnitNormal + RandomVectorInUnitSphere(seed);
			if (dot(scatteredDirection, scatteredDirection) < 1e-3) {
				scatteredDirection = hit.UnitNormal;
			}
		}

		scatteredRay = Ray(hit.Position, scatteredDirection, material.RefractiveIndex);
		return true;
	}
}

bool RayIntersection(Sphere sphere, Ray ray, Interval tInterval, inout HitPayload hit)
{
	vec3 sphereDirection = vec3(sphere.Position) - ray.Origin;

	float a = dot(ray.Direction, ray.Direction);
	float b = -2 * dot(ray.Direction, sphereDirection);
	float c = dot(sphereDirection, sphereDirection) - sphere.Radius * sphere.Radius;
	float discriminant = b * b - 4 * a * c;

	if (discriminant < 0.0f)
		return false;

	float discriminantSqrt = sqrt(discriminant);

	float t = (-b - discriminantSqrt) / (2 * a);

	if (!DoesIntervalContain(t, tInterval))
	{
		t = (-b + discriminantSqrt) / (2 * a);
		if (!DoesIntervalContain(t, tInterval))
			return false;
	}
		
	hit.T = t;
	hit.Position = ray.Origin + t * ray.Direction;
	vec3 outwardNormal = (hit.Position - vec3(sphere.Position)) / sphere.Radius;
	SetFaceNormal(hit, ray, outwardNormal);
	hit.MaterialIndex = sphere.MaterialIndex;

	return true;
}

HitPayload TraceRay(Ray ray)
{
	float tMin = 1e-3;

	HitPayload hit;
	hit.T = -1;
	float closest = 1.0/0.0;

	for (uint i = 0; i < spheresCount; i++)
	{
		Sphere sphere = spheres[i];
		if (RayIntersection(sphere, ray, Interval(tMin, closest), hit))
		{
			closest = hit.T;
		}
	}

	return hit;
}

bool IsPixelOnFocusSphere(uint x, uint y)
{
	vec3 rayOrigin = vec3(camera.Position);
	vec3 rayDirection = normalize(GetVirtualPixelPosition(x, y) - rayOrigin);
	Ray ray = { rayOrigin, rayDirection, 1.0f };

	HitPayload hit = TraceRay(ray);

	return abs(hit.T - camera.FocusDistance) <= 0.01;
}

vec3 ComputeIllumination(Ray incidentRay, HitPayload hit, inout uint seed)
{
	vec3 illumination = vec3(0.0);
	vec3 contribution = { 1.0, 1.0, 1.0 };

	for (uint i = 0; i <= settings.Depth; i++)
	{
		vec3 ownLight = vec3(materials[hit.MaterialIndex].EmittedColor * PI);

		if (i == settings.Depth)
		{
			illumination += ownLight * contribution;
			break;
		}

		Ray scatteredRay;
		ScatterRay(incidentRay, hit, scatteredRay, seed);
		HitPayload nextHit = TraceRay(scatteredRay);

		if (nextHit.T == -1)
		{
			illumination += (ownLight + vec3(skyColor) * PI) * contribution;
			break;
		}

		vec3 directIllumination = vec3(materials[nextHit.MaterialIndex].EmittedColor);

		seed++;

		illumination += (ownLight + directIllumination) * contribution;

		contribution *= vec3(materials[nextHit.MaterialIndex].Albedo);

		incidentRay = scatteredRay;
		hit = nextHit;
	}

	return illumination;
}

vec3 ComputePixelLuminance(uint x, uint y, inout uint seed)
{
	vec3 rayOrigin = GetRayOrigin(seed);
	vec3 rayDirection = normalize(GetVirtualPixelPosition(x, y, seed) - rayOrigin);
	Ray ray = { rayOrigin, rayDirection, 1.0f };

	HitPayload hit = TraceRay(ray);
		
	if (hit.T <= 0.0f)
		return vec3(skyColor);

	vec3 illumination = ComputeIllumination(ray, hit, seed);
	vec3 luminance = illumination * vec3(materials[hit.MaterialIndex].Albedo / PI);

	return luminance;
}

vec4 GetRayTracedPixel(uint x, uint y)
{
	uint seed = x + y * imageSize(outputImage).x;
	seed *= frameIndex;

	vec3 instantLuminance = ComputePixelLuminance(x, y, seed);

	if (frameIndex == 1)
		integratedLuminance[x + y * imageSize(outputImage).x] = vec4(0.0);

	integratedLuminance[x + y * imageSize(outputImage).x] += vec4(instantLuminance, 0.0);

	vec3 continuousLuminance = vec3(integratedLuminance[x + y * imageSize(outputImage).x] / frameIndex);
	return GetPixel(continuousLuminance);
}

vec4 GetPreviewPixel(uint x, uint y)
{
	vec3 rayOrigin = vec3(camera.Position);
	vec3 rayDirection = normalize(GetVirtualPixelPosition(x, y) - rayOrigin);
	Ray ray = { rayOrigin, rayDirection, 1.0f };

	HitPayload hit = TraceRay(ray);

	if (hit.T <= 0)
	{
		switch (settings.PreviewMode)
		{
		case 1:
			return vec4(0.0, 0.0, 0.0, 1.0);
		case 2:
			return GetPixel(vec3(skyColor));
		}
	}

	if (settings.PreviewMode == 1)
	{
		float depthInvert = hit.T < 1.0 ? 1.0 : 1.0 / hit.T;
		return vec4(depthInvert, depthInvert, depthInvert, 1.0);
	}

	vec3 illumination;

	for (uint i = 0; i < spheresCount; i++)
	{
		Sphere sphere = spheres[i];

		vec3 hitToLight = vec3(sphere.Position) - hit.Position;

		float cosAngle = dot(hit.UnitNormal, normalize(hitToLight));

		illumination += vec3(materials[sphere.MaterialIndex].EmittedColor) / 2 * GetVisibleSurface(sphere) * cosAngle / dot(hitToLight, hitToLight);
	}

	vec3 luminance = illumination * vec3(materials[hit.MaterialIndex].Albedo / PI);

	return GetPixel(luminance);
}

void main()
{
    vec4 value = { 0.0, 0.0, 0.0, 1.0 };
    ivec2 pixelCoord = ivec2(gl_GlobalInvocationID.xy);
	
    if (settings.Preview)
		value = GetPreviewPixel(pixelCoord.x, pixelCoord.y);
	else
		value = GetRayTracedPixel(pixelCoord.x, pixelCoord.y);

	if (settings.FocusPeaking && IsPixelOnFocusSphere(pixelCoord.x, pixelCoord.y))
		value = vec4(1.0, 0.0, 0.0, 1.0);

    imageStore(outputImage, pixelCoord, value);
}