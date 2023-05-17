#pragma once

#include "common_macro.hpp"


class RenderToImage {
 public:
    RenderToImage() {
        GLint viewport_size[4];
        glGetIntegerv(GL_VIEWPORT, viewport_size);
        GL_CHECK();
        width_ = viewport_size[2];
        height_ = viewport_size[3];
        // Generate Frame Buffer
	    glGenFramebuffers(1, &frame_buffer_id_);
        // Generate Texture
		glGenTextures(1, &texture_id_);
        GL_CHECK();
		glBindTexture(GL_TEXTURE_2D, texture_id_);
		// Give an empty image to OpenGL ( the last "0" )
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, width_, height_, 0, GL_RGBA, GL_FLOAT, 0);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	    
        glBindFramebuffer(GL_FRAMEBUFFER, frame_buffer_id_);
        glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, texture_id_, 0);
        
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glBindTexture(GL_TEXTURE_2D, 0);
        GL_CHECK();
    }
    ~RenderToImage() {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glDeleteFramebuffers(1, &frame_buffer_id_);
    }
    
    int get_frame_buffer() {
        return frame_buffer_id_;
    }

    int get_texture() {
        return texture_id_;
    }
    
    int screen_capture(std::string image_path) {
        return read_from_frame_buffer(image_path);
    }

    int draw_image(std::string image_path) {
        glBindFramebuffer(GL_FRAMEBUFFER, frame_buffer_id_);
        GL_CHECK_RET(-1);
		GLenum drawBuffers[2] = {GL_COLOR_ATTACHMENT0, 0};
        // Draw to the color attachment of the frame buffer
		glDrawBuffers(1, drawBuffers);
        GL_CHECK_RET(-1);
        
        CHECK_EQ_RET(read_from_frame_buffer(image_path), GL_TRUE, -1);
        
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        return GL_TRUE;
    }

 private:
    int read_from_frame_buffer(std::string image_path) {
        // Read pixels from color attachment of the bound framebuffer
        std::vector<float> f_bytes(width_ * height_ * 4);
	    // glReadBuffer(GL_COLOR_ATTACHMENT0);
        GL_CHECK_RET(-1);
        glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
	    glReadPixels(0, 0, width_, height_, GL_RGBA, GL_FLOAT, f_bytes.data());
        GL_CHECK_RET(-1);
        std::vector<unsigned char> bytes(width_ * height_ * 4);
        // scale to 255 and turn upside down
        for (int h = 0; h < height_; h++) {
            for (int w = 0; w < width_; w++) {
                for (int c = 0; c < 4; c++) {
                    int idx0 = (height_ - 1 - h) * width_ * 4 + w * 4 + c;
                    bytes[idx0] = f_bytes[idx0] * 255;
                }
            }
        }
        // for (int i = 0; i < bytes.size(); i++) {
        //     bytes[i] = f_bytes[i] * 255;
        // }
		stbi_write_png(image_path.c_str(), width_, height_, 4, bytes.data(), width_ * 4);
        return GL_TRUE;
    }

 private:
    GLuint frame_buffer_id_ = 0;
    GLuint texture_id_ = 0;
    int width_ = 0, height_ = 0;
};
