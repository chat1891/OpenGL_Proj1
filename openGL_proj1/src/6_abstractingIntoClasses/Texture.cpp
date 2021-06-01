#include "Texture.h"
#include "Vendor/stb_image/stb_image.h"

Texture::Texture(const std::string& path)
	:m_RendererID(0), m_FilePath(path), m_LocalBuffer(nullptr), m_Width(0), m_Height(0), m_BitPerPixel(0)
{
	//flips texture vertically upside down
	//coz openGL expects texture pixels starts bottom left, not top left
	//typically when we load images, scan lines usually from top to bottom
	stbi_set_flip_vertically_on_load(1);

	//desired channels: r,g,b,a
	m_LocalBuffer = stbi_load(path.c_str(), &m_Width, &m_Height, &m_BitPerPixel, 4);

	GLCall(glGenTextures(1, &m_RendererID));
	GLCall(glBindTexture(GL_TEXTURE_2D, m_RendererID));

	//linear: linear filtering
	//determine what happens when a u,v coord doesn't precise to a center of pixel. to a individual color value in the texture.
	//with linear filter, in that case, not just get color of that pixel but avg of that pixel and its immediate neighbors
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
	//Render texture on area that is larger in pixels in actual texture size, so need to scale it up
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
	//Clamp: not extend the area
	//S, T: U,V direction
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));

	GLCall(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, m_Width, m_Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, m_LocalBuffer));
	GLCall(glBindTexture(GL_TEXTURE_2D, 0));

	if (m_LocalBuffer)
	{
		stbi_image_free(m_LocalBuffer);
	}
}

Texture::~Texture()
{
	GLCall(glDeleteTextures(1, &m_RendererID));
}

void Texture::Bind(unsigned int slot) const
{
	GLCall(glActiveTexture(GL_TEXTURE0 + slot));
	GLCall(glBindTexture(GL_TEXTURE_2D, m_RendererID));

}

void Texture::Unbind() const
{
	GLCall(glBindTexture(GL_TEXTURE_2D, 0));
}
