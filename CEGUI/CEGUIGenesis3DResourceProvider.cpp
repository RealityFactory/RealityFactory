/************************************************************************************//**
 * @file CEGUIGenesis3DResourceProvider.cpp
 * @brief Implementation of Genesis3D Resource Provider for CEGUI
 * @author Daniel Queteschiner
 * @date July 2009
 *//*
 * Copyright (c) 2009 Daniel Queteschiner; All rights reserved.
 ****************************************************************************************/

#include <CEGUIExceptions.h>
#include "CEGUIGenesis3DResourceProvider.h"


// Start of CEGUI namespace section
namespace CEGUI
{

Genesis3DResourceProvider::Genesis3DResourceProvider() : ResourceProvider()
{
	d_vFile = NULL;
}


Genesis3DResourceProvider::Genesis3DResourceProvider(geVFile *vFile) : ResourceProvider()
{
	d_vFile = vFile;
}


Genesis3DResourceProvider::~Genesis3DResourceProvider()
{
}


void Genesis3DResourceProvider::loadRawDataContainer(
	const String& filename,
	RawDataContainer& output,
	const String& resourceGroup)
{
	if(filename.empty())
	{
		throw InvalidRequestException(
			"Genesis3DResourceProvider::load - Filename supplied for data loading must be valid");
	}

	String final_filename(getFinalFilename(filename, resourceGroup));

	geVFile *data_file = geVFile_OpenNewSystem(NULL, GE_VFILE_TYPE_DOS, final_filename.c_str(), NULL, GE_VFILE_OPEN_READONLY);

	if(data_file == NULL && d_vFile)
	{
		data_file = geVFile_Open(d_vFile, final_filename.c_str(), GE_VFILE_OPEN_READONLY);
	}

	if(data_file == NULL)
	{
		throw InvalidRequestException("Genesis3DResourceProvider::load - " + filename + " does not exist");
	}

	long size;
	geVFile_Size(data_file, &size);

	unsigned char* buffer = new unsigned char [size];

	if(geVFile_Read(data_file, (void*)buffer, size) == GE_FALSE)
	{
		delete [] buffer;
		throw GenericException("Genesis3DResourceProvider::loadRawDataContainer - Problem reading " + filename);
	}

	geVFile_Close(data_file);

	output.setData(buffer);
	output.setSize(size);
}


void Genesis3DResourceProvider::unloadRawDataContainer(RawDataContainer& data)
{
	uint8* ptr = data.getDataPtr();
	delete [] ptr;
	data.setData(0);
	data.setSize(0);
}


void Genesis3DResourceProvider::setResourceGroupDirectory(const String& resourceGroup, const String& directory)
{
	d_resourceGroups[resourceGroup] = directory;
}


const String& Genesis3DResourceProvider::getResourceGroupDirectory(const String& resourceGroup)
{
	return d_resourceGroups[resourceGroup];
}


void Genesis3DResourceProvider::clearResourceGroupDirectory(const String& resourceGroup)
{
	ResourceGroupMap::iterator iter = d_resourceGroups.find(resourceGroup);

	if(iter != d_resourceGroups.end())
		d_resourceGroups.erase(iter);
}


String Genesis3DResourceProvider::getFinalFilename(const String& filename, const String& resourceGroup) const
{
	String final_filename;

	// look up resource group directory
	ResourceGroupMap::const_iterator iter =
		d_resourceGroups.find(resourceGroup.empty() ? d_defaultResourceGroup : resourceGroup);

	// if there was an entry for this group, use its directory as the
	// first part of the filename
	if (iter != d_resourceGroups.end())
		final_filename = (*iter).second;

	// append the filename part that we were passed
	final_filename += filename;

	// return result
	return final_filename;
}

}
