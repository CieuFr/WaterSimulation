#ifndef __IMAGE_HPP__
#define __IMAGE_HPP__

#include "file_path.hpp"


struct Image
{
	Image() = default;
	~Image();

	bool load( const FilePath & p_filePath );

	int				_width		= 0;
	int				_height		= 0;
	int				_nbChannels = 0;
	unsigned char * _pixels		= nullptr;
};


#endif // __IMAGE_HPP__
