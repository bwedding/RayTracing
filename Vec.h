#pragma once
struct vec3
{
	float x;
	float y;
	float z;

	vec3 operator+(const vec3& other) const
	{
		return { x + other.x, y + other.y, z + other.z };
	}

	vec3 operator-(const vec3& other) const
	{
		return { x - other.x, y - other.y, z - other.z };
	}

	vec3 operator-() const
	{
		return { -x, -y, -z };
	}

	vec3 operator*(const float& scalar) const
	{
		return { x * scalar, y * scalar, z * scalar };
	}

	vec3 operator/(const float& scalar) const
	{
		return { x / scalar, y / scalar, z / scalar };
	}

	float dot(const vec3& other) const
	{
		return x * other.x + y * other.y + z * other.z;
	}

	float len() const
	{
		return sqrtf(this->dot(*this));
	}

	void normalize()
	{
		float len = this->len();
		x /= len;
		y /= len;
		z /= len;
	}
};