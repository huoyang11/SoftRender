#include "render.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

static bool point_in_triangle(glm::vec3 A, glm::vec3 B, glm::vec3 C, glm::vec3 p)
{
	double AB = (B.x - A.x) * (p.y - A.y) - (B.y - A.y) * (p.x - A.x);
	double BC = (C.x - B.x) * (p.y - B.y) - (C.y - B.y) * (p.x - B.x);
	double CA = (A.x - C.x) * (p.y - C.y) - (A.y - C.y) * (p.x - C.x);

	if (((AB > 0 && BC > 0 && CA > 0) || (AB < 0 && BC < 0 && CA < 0)))
		return true;
	else
		return false;
}

static float cross(const glm::vec2& A, glm::vec2& B)
{
	return A.x * B.y - A.y * B.x;
}

vertex_buffer::vertex_buffer(void* b, uint32_t size)
{
	this->buffer = b;
    this->size = size;
	this->item_size = 0;
}

void* vertex_buffer::get_buffer() const
{
	return buffer;
}

void vertex_buffer::set_item_size(uint32_t item_size)
{
	this->item_size = item_size;
}

uint32_t vertex_buffer::get_item_size() const
{
	return item_size;
}

uint32_t vertex_buffer::get_size() const
{
	return size;
}

void* vertex_buffer::get_index(uint32_t index) const
{
	assert(item_size != 0);
	return (void *)((uint8_t *)buffer + index * item_size);
}

vertex_buffer::~vertex_buffer()
{

}

vertex_index_buffer::vertex_index_buffer(uint32_t* buffer, uint32_t size)
{
	this->buffer = buffer;
	this->size = size;
}

vertex_index_buffer::~vertex_index_buffer()
{

}

uint32_t* vertex_index_buffer::get_buffer() const
{
	return buffer;
}

uint32_t vertex_index_buffer::get_size() const
{
	return size;
}

float* render::zBuffer1;
float* render::zBuffer2;
//std::vector<VS_OUT> render::vs_outs;
std::vector<VS_OUT> render::fragments;
std::vector<FG_OUT> render::fg_outs;
std::vector<triangle> render::triangles;
std::unordered_map<std::string, void*> render::unifroms;

render::render() : m_img(1280,720)
{
	zBuffer1 = (float*)calloc(1, m_w * m_h * sizeof(float));
	zBuffer2 = (float*)calloc(1, m_w * m_h * sizeof(float));
}

render::~render()
{

}

glm::vec4 render::viewport_vec(const glm::vec4 &v)
{
	float x = (v.x / 2.0 + 0.5) * m_w;
	float y = m_h - (v.y / 2.0 + 0.5) * m_h;

	y = glm::clamp(y, 0.f, (float)m_h);

	return glm::vec4(x, y, v.z, 1.0f);
}

void render::set_vertex_shader_callback(vertex_shader_callback fn)
{
	m_vsfn = fn;
}

void render::set_fragment_shader_callback(fragment_shader_callback fn)
{
	m_fsfn = fn;
}

void render::set_unifrom(std::string name, void* data)
{
	unifroms[name] = data;
}

void* render::get_unifrom(std::string name)
{
	return unifroms[name];
}

void render::draw_line(const glm::vec2 &p1,const glm::vec2 &p2)
{
    //Bresenham算法
	int x1 = p1.x, y1 = p1.y, x2 = p2.x, y2 = p2.y;
	int dx = x2 - x1, dy = y2 - y1;
	int s1 = (dx >= 0 ? 1 : -1), s2 = (dy >= 0 ? 1 : -1);
	dx = abs(dx); dy = abs(dy);
    m_img.set_color(x1, y1,color(255,0,0,255));
	int xbase = x1, ybase = y1;
	if (dx >= dy)
	{
		int e = (dy << 1) - dx, deta1 = dy << 1, deta2 = (dy - dx) << 1;
		while (xbase != x2)
		{
			if (e >= 0)//y方向增量为1
				xbase += s1, ybase += s2, e += deta2;
			else xbase += s1, e += deta1;
            m_img.set_color(xbase, ybase,color(255,0,0,255));
		}
	}
	else
	{
		int e = (dx << 1) - dy, deta1 = dx << 1, deta2 = (dx - dy) << 1;
		while (ybase != y2)
		{
			if (e >= 0)//x方向增量为1
				xbase += s1, ybase += s2, e += deta2;
			else ybase += s2, e += deta1;
            m_img.set_color(xbase, ybase,color(255,0,0,255));
		}
	}
}

void render::draw_triangle()
{
	//顶点着色器
	vertex();
	//裁剪
	//cliping();

#if 0
	//背面剔除
	std::vector<triangle> tem_triangle;
	for (auto& it : triangles) {
		glm::vec2 AB = glm::vec2(it.vertex[1].position.x, it.vertex[1].position.y) - glm::vec2(it.vertex[0].position.x, it.vertex[0].position.y);
		glm::vec2 BC = glm::vec2(it.vertex[2].position.x, it.vertex[2].position.y) - glm::vec2(it.vertex[1].position.x, it.vertex[1].position.y);

		if (cross(AB, BC) > 0) tem_triangle.push_back(it);
	}

	triangles = tem_triangle;
	tem_triangle.clear();
#endif
	//透视除法
	for (auto& it : triangles) {
		it.vertex[0].position.x = it.vertex[0].position.x / it.vertex[0].position.w;
		it.vertex[0].position.y = it.vertex[0].position.y / it.vertex[0].position.w;
		//it.vertex[0].position.z = it.vertex[0].position.z / it.vertex[0].position.w;
		it.vertex[0].position.w = 1.0f;

		it.vertex[1].position.x = it.vertex[1].position.x / it.vertex[1].position.w;
		it.vertex[1].position.y = it.vertex[1].position.y / it.vertex[1].position.w;
		//it.vertex[1].position.z = it.vertex[1].position.z / it.vertex[1].position.w;
		it.vertex[1].position.w = 1.0f;

		it.vertex[2].position.x = it.vertex[2].position.x / it.vertex[2].position.w;
		it.vertex[2].position.y = it.vertex[2].position.y / it.vertex[2].position.w;
		//it.vertex[2].position.z = it.vertex[2].position.z / it.vertex[2].position.w;
		it.vertex[2].position.w = 1.0f;
	}

	//把z值变换到[0,1]
	//for (auto& it : triangles) {
	//	it.vertex[0].position.z = (it.vertex->position.z + 1.0f) / 2.0f;
	//	it.vertex[1].position.z = (it.vertex->position.z + 1.0f) / 2.0f;
	//	it.vertex[2].position.z = (it.vertex->position.z + 1.0f) / 2.0f;
	//}


	//屏幕映射
	screen_mapping();

	//for (auto& it : triangles) {
	//	draw_line(it.vertex[0].position, it.vertex[1].position);
	//	draw_line(it.vertex[1].position, it.vertex[2].position);
	//	draw_line(it.vertex[2].position, it.vertex[0].position);
	//}
#if 1
	//三角形遍历
	for_each_triangle();
	//片元着色器
	fragment();
	//合并
	merge();
#endif

	unifroms.clear();
	//vs_outs.clear();
	fragments.clear();
	fg_outs.clear();
	triangles.clear();
}

void render::draw_triangle_ex()
{
	float* zBuffer = zBuffer1;
	uint32_t* ibuffer = m_vib->get_buffer();
	for (int i = 0; i < m_vib->get_size() / 3; i++) {
		triangle _triangle;

		m_vsfn(this, VS_IN{ m_vb->get_index(ibuffer[i * 3]) }, _triangle.vertex[0]);
		m_vsfn(this, VS_IN{ m_vb->get_index(ibuffer[i * 3 + 1]) }, _triangle.vertex[1]);
		m_vsfn(this, VS_IN{ m_vb->get_index(ibuffer[i * 3 + 2]) }, _triangle.vertex[2]);

		//背面剔除
		glm::vec2 AB = glm::vec2(_triangle.vertex[1].position.x, _triangle.vertex[1].position.y) - glm::vec2(_triangle.vertex[0].position.x, _triangle.vertex[0].position.y);
		glm::vec2 BC = glm::vec2(_triangle.vertex[2].position.x, _triangle.vertex[2].position.y) - glm::vec2(_triangle.vertex[1].position.x, _triangle.vertex[1].position.y);
		if (cross(AB, BC) < 0.f) continue;

		//透视除法
		for (int i = 0; i < 3; i++) {
			_triangle.vertex[i].position.x = _triangle.vertex[i].position.x / _triangle.vertex[i].position.w;
			_triangle.vertex[i].position.y = _triangle.vertex[i].position.y / _triangle.vertex[i].position.w;
			_triangle.vertex[i].position.w = 1.0f;
		}

		//屏幕映射
		_triangle.vertex[0].position = viewport_vec(_triangle.vertex[0].position);
		_triangle.vertex[1].position = viewport_vec(_triangle.vertex[1].position);
		_triangle.vertex[2].position = viewport_vec(_triangle.vertex[2].position);

		//光栅化
		glm::vec2 min;
		glm::vec2 max;

		min.x = std::min(std::min(_triangle.vertex[0].position.x, _triangle.vertex[1].position.x), _triangle.vertex[2].position.x);
		min.y = std::min(std::min(_triangle.vertex[0].position.y, _triangle.vertex[1].position.y), _triangle.vertex[2].position.y);

		max.x = std::max(std::max(_triangle.vertex[0].position.x, _triangle.vertex[1].position.x), _triangle.vertex[2].position.x);
		max.y = std::max(std::max(_triangle.vertex[0].position.y, _triangle.vertex[1].position.y), _triangle.vertex[2].position.y);

		for (int x = min.x; x < max.x; x++) {
			for (int y = min.y; y < max.y; y++) {
				glm::vec3 point = { (float)x, (float)y, 0.f };
				if (!point_in_triangle(_triangle.vertex[0].position, _triangle.vertex[1].position, _triangle.vertex[2].position, point)) continue;

				glm::vec2 v[3] = {
					{_triangle.vertex[0].position.x, _triangle.vertex[0].position.y},
					{_triangle.vertex[1].position.x, _triangle.vertex[1].position.y},
					{_triangle.vertex[2].position.x, _triangle.vertex[2].position.y},
				};

				glm::vec2 p = { point.x,point.y };
				//求重心坐标
				float area_twice = std::abs(cross((v[1] - v[0]), (v[2] - v[0])));
				float a = std::abs(cross((v[1] - p), (v[2] - p)) / area_twice);
				float b = std::abs(cross((v[0] - p), (v[2] - p)) / area_twice);
				float c = std::abs(cross((v[0] - p), (v[1] - p)) / area_twice);
				//透视矫正
				// 1/z = a/za + b/zb + c/zc
				// z = 1 / (a/za + b/zb + c/zc)
				point.z = 1.0f / (a / _triangle.vertex[0].position.z + b / _triangle.vertex[1].position.z + c / _triangle.vertex[2].position.z);

				int index = (int)(point.x + point.y * m_w);
				if (zBuffer[index] == 0.f) {
					VS_OUT vs_out = get_point_vs_out(a, b, c, _triangle, point);
					FG_OUT out = { vs_out.position };
					m_fsfn(this, vs_out, out);
					m_img.set_color(out.position.x, out.position.y, color(out.color.x * 255, out.color.y * 255, out.color.z * 255, out.color.w));
					zBuffer[index] = point.z;
					continue;
				}
				if (zBuffer[index] > point.z) {
					VS_OUT vs_out = get_point_vs_out(a, b, c, _triangle, point);
					FG_OUT out = { vs_out.position };
					m_fsfn(this, vs_out, out);
					m_img.set_color(out.position.x, out.position.y, color(out.color.x * 255, out.color.y * 255, out.color.z * 255, out.color.w));
					zBuffer[index] = point.z;
				}
			}
		}
	}
}

const image &render::get_image() const
{
    return m_img;
}

void render::viewport(uint16_t w,uint16_t h)
{
	m_w = w;
	m_h = h;

	m_img.resize(w,h);

	zBuffer1 = (float*)realloc(zBuffer1, m_w * m_h * sizeof(float));
	zBuffer2 = (float*)realloc(zBuffer2, m_w * m_h * sizeof(float));
}

struct texture render::load_texture(const std::string& path)
{
	int width, height, nrComponents;
	unsigned char* data = stbi_load(path.c_str(), &width, &height, &nrComponents, 0);
	return texture{data,width,height};
}

glm::vec4 render::texture2D(struct texture* t, const glm::vec2& uv)
{
	if (isnan(uv.x) || isnan(uv.y)) return glm::vec4(0.f, 0.f, 0.f, 1.0f);

	int u = (int)(uv.x * t->w);
	int v = (int)(uv.y * t->h);

	int index = u + v * t->w;
	uint8_t* p = (uint8_t*)(((uint32_t*)t->data) + index);

	return glm::vec4(p[0] / 255.0f, p[1] / 255.0f, p[2] / 255.0f, 1.0f);
}

void render::set_vertex_buffer(vertex_buffer* vb)
{
    m_vb = vb;
}

void render::set_vertex_index_buffer(vertex_index_buffer* vib)
{
	m_vib = vib;
}

void render::clean()
{
    m_img.clean();

	memset(zBuffer1, 0, m_w * m_h * sizeof(float));
	memset(zBuffer2, 0, m_w * m_h * sizeof(float));
}

void render::vertex()
{
	uint32_t* ibuffer = m_vib->get_buffer();
	for (int i = 0; i < m_vib->get_size() / 3;i++) {
		VS_OUT out_data1;
		VS_OUT out_data2;
		VS_OUT out_data3;

		m_vsfn(this, VS_IN{ m_vb->get_index(ibuffer[i * 3]) }, out_data1);
		m_vsfn(this, VS_IN{ m_vb->get_index(ibuffer[i * 3 + 1]) }, out_data2);
		m_vsfn(this, VS_IN{ m_vb->get_index(ibuffer[i * 3 + 2]) }, out_data3);

		triangles.push_back({ out_data1 ,out_data2 ,out_data3 });
		//vs_outs.push_back(out_data);
	}
}

void render::cliping()
{
}

void render::screen_mapping()
{
	for (auto& it : triangles) {
		it.vertex[0].position = viewport_vec(it.vertex[0].position);
		it.vertex[1].position = viewport_vec(it.vertex[1].position);
		it.vertex[2].position = viewport_vec(it.vertex[2].position);
	}
}

void render::interpolation(const VS_OUT& A, const VS_OUT& B, const VS_OUT& C, glm::vec3& point)
{
	//float c = ((A.position.y - B.position.y) * point.x +
	//	(B.position.x - A.position.x) * point.y +
	//	A.position.x * B.position.y - B.position.x * A.position.y) /
	//	((A.position.y - B.position.y) * C.position.x +
	//		(B.position.x - A.position.x) * C.position.y +
	//		A.position.x * B.position.y - B.position.x * A.position.y);
	//
	//float b = ((A.position.y - C.position.y) * point.x +
	//	(C.position.x - A.position.x) * point.y +
	//	A.position.x * C.position.y - C.position.x * A.position.y) /
	//	((A.position.y - C.position.y) * B.position.x +
	//		(C.position.x - A.position.x) * B.position.y +
	//		A.position.x * C.position.y - C.position.x * A.position.y);
	//
	//float a = 1.0f - b - c;

	glm::vec2 v[3] = {
		{A.position.x, A.position.y},
		{B.position.x, B.position.y},
		{C.position.x, C.position.y},
	};

	glm::vec2 p = {point.x,point.y};

	float area_twice = std::abs(cross((v[1] - v[0]),(v[2] - v[0])));
	float a = std::abs(cross((v[1] - p), (v[2] - p)) / area_twice);
	float b = std::abs(cross((v[0] - p), (v[2] - p)) / area_twice);
	float c = std::abs(cross((v[0] - p), (v[1] - p)) / area_twice);

	// 1/z = a/za + b/zb + c/zc
	// z = 1 / (a/za + b/zb + c/zc)
	VS_OUT out;
	point.z = 1.0f / (a / A.position.z + b / B.position.z + c / C.position.z);
	out.position = glm::vec4(point.x, point.y, point.z, 1.0f);

	// u/z = au/za + bu/zb + cu/zc
	int size = A.out_to_fragment.size();
	for (int i = 0; i < size; i++) {
		out.out_to_fragment.push_back(
				(a * A.out_to_fragment.at(i) / A.position.z +
				 b * B.out_to_fragment.at(i) / B.position.z +
				 c * C.out_to_fragment.at(i) / C.position.z)*
				 point.z
		);
	}

	fragments.push_back(out);
}

void render::scanning_triangle(const VS_OUT& A,const VS_OUT& B,const VS_OUT& C)
{
	glm::vec2 min;
	glm::vec2 max;

	min.x = std::min(std::min(A.position.x, B.position.x), C.position.x);
	min.y = std::min(std::min(A.position.y, B.position.y), C.position.y);

	max.x = std::max(std::max(A.position.x, B.position.x), C.position.x);
	max.y = std::max(std::max(A.position.y, B.position.y), C.position.y);

	//memset(zBuffer2,0, m_w * m_h * sizeof(float));
	for (int x = min.x; x < max.x; x++) {
		for (int y = min.y; y < max.y; y++) {
			glm::vec3 point = { (float)x, (float)y, 0.f };
			if (!point_in_triangle(A.position, B.position, C.position, point)) continue;

			//if (zBuffer2[x + y * m_w] > point.z) continue;

			interpolation(A,B,C,point);
			//zBuffer2[x + y * m_w] = point.z;
		}
	}
}

void render::for_each_triangle()
{
	for (auto& it : triangles) {
		scanning_triangle(it.vertex[0], it.vertex[1], it.vertex[2]);
	}
}

void render::fragment()
{
	for (const auto& it : fragments) {
		FG_OUT out = {it.position};
		m_fsfn(this,it, out);
		fg_outs.push_back(out);
	}
}

void render::merge()
{
	float* zBuffer = zBuffer1;
	for (const auto& it : fg_outs) {
		int index = (int)(it.position.x + it.position.y * m_w);
		float zVal = zBuffer[index];
		if (zVal == 0.f) {
			m_img.set_color(it.position.x, it.position.y, color(it.color.x * 255, it.color.y * 255, it.color.z * 255, it.color.w));
			zBuffer[index] = it.position.z;
			continue;
		}

		if (zVal > it.position.z) {
			m_img.set_color(it.position.x, it.position.y, color(it.color.x * 255, it.color.y * 255, it.color.z * 255, it.color.w));
			zBuffer[index] = it.position.z;
		}
	}
}

VS_OUT render::get_point_vs_out(float a, float b, float c, const triangle& t, const glm::vec3& point)
{
	VS_OUT out;
	out.position = glm::vec4(point.x, point.y, point.z, 1.0f);

	// u/z = au/za + bu/zb + cu/zc
	int size = t.vertex[0].out_to_fragment.size();
	for (int i = 0; i < size; i++) {
		out.out_to_fragment.push_back(
			(a * t.vertex[0].out_to_fragment.at(i) / t.vertex[0].position.z +
				b * t.vertex[1].out_to_fragment.at(i) / t.vertex[1].position.z +
				c * t.vertex[2].out_to_fragment.at(i) / t.vertex[2].position.z) *
			point.z
		);
	}

	return out;
}
