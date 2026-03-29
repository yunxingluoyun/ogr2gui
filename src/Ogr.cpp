/*****************************************************************************
 *	ogr2gui is an application used to convert and manipulate geospatial
 *	data. It is based on the "OGR Simple Feature Library" from the 
 *	"Geospatial Data Abstraction Library" <http://gdal.org>.
 *
 *	Copyright (c) 2009 Inventis <mailto:developpement@inventis.ca>
 *
 *	This program is free software: you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation, either version 3 of the License, or
 *	(at your option) any later version.
 *
 *	This program is distributed in the hope that it will be useful,
 *	but WITHOUT ANY WARRANTY; without even the implied warranty of
 *	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *	GNU General Public License for more details.
 *
 *	You should have received a copy of the GNU General Public License
 *	along with this program.  If not, see <http://www.gnu.org/licenses/>
 *****************************************************************************/

/*!
 *	\file Ogr.cpp
 *	\brief OGR C API
 *	\author Olivier Pilotte [ Inventis ]
 *	\version 0.6
 *	\date 27/10/09
 */

#include "../inc/Ogr.h"

#include <cstdio>

Ogr::Ogr( void ) :
	formatDriver( NULL ),
	sourceData( NULL ),
	targetData( NULL ),
	squeryLayer( NULL ),
	sourceLayer( NULL ),
	targetLayer( NULL ),
	sourceSRS( NULL ),
	targetSRS( NULL ),
	sourceLayerDefn( NULL ),
	sourceLayerGeom( wkbUnknown ),
	sourceGeom( NULL ),
	targetLayerWasExisting( false )
{
	OGRRegisterAll();
}

Ogr::~Ogr( void )
{
	CloseTarget();
	CloseSource();
}

bool Ogr::OpenSource( string filename, string &epsg, string &query, string &error )
{
	CloseSource();

	error.clear();
	sourceSRS = NULL;

	sourceName = filename;

	sourceData = OGROpen( sourceName.c_str(), 0, NULL );

	if( sourceData != NULL )
	{
		sourceLayer = OGR_DS_GetLayer( sourceData, 0 );

		if( sourceLayer != NULL )
		{
			sourceLayerDefn = OGR_L_GetLayerDefn( sourceLayer );

			sourceLayerName = OGR_FD_GetName( sourceLayerDefn );

			sourceLayerGeom = OGR_FD_GetGeomType( sourceLayerDefn );

			sourceGeom = OGR_L_GetSpatialFilter( sourceLayer );

			sourceSRS = OGR_L_GetSpatialRef( sourceLayer );

			if( sourceSRS != NULL && ! Error( OSRAutoIdentifyEPSG( sourceSRS ), error ) )
			{
				epsg = OSRGetAttrValue( sourceSRS, "AUTHORITY", 1 );
			}
			else
			{
				error = "unable to open source spatial reference";
			}

			query = "SELECT * FROM " + sourceLayerName;
		}
		else
		{
			error = "unable to open source layer";

			return false;
		}
	}
	else
	{
		error = "unable to open source data";

		return false;
	}

	return true;
}

bool Ogr::CloseSource( void )
{
	if( sourceData != NULL && squeryLayer != NULL )
	{
		OGR_DS_ReleaseResultSet( sourceData, squeryLayer );
		squeryLayer = NULL;
	}

	if( sourceData != NULL )
	{
		OGR_DS_Destroy( sourceData );
		sourceData = NULL;
		sourceLayer = NULL;
		sourceLayerDefn = NULL;
		sourceSRS = NULL;
		sourceGeom = NULL;

		sourceName.clear();
		sourceLayerName.clear();
	}
	else
	{
		return false;
	}

	return true;
}

bool Ogr::OpenDriver( const string &drivername )
{
	error.clear();
	formatDriver = OGRGetDriverByName( drivername.c_str() );

	if( formatDriver == NULL )
	{
		error = "unable to find driver";

		return false;
	}

	return true;
}

bool Ogr::OpenTarget( const string &filename, int projection, bool update )
{
	struct stat fileInfo;

	CloseTarget();

	error.clear();
	targetSRS = NULL;
	targetLayerWasExisting = false;
	
	targetName = filename;

	if( projection > 0 )
	{
		targetSRS = OSRNewSpatialReference( NULL );

		if( Error( OSRImportFromEPSG( targetSRS, projection ), error ) )
		{
			error.insert( 0, "unable to create spatial reference : " );
			return false;
		}
	}

	if( update )
	{
		if( stat( targetName.c_str(), &fileInfo ) == 0 )
		{
			targetData = OGR_Dr_Open( formatDriver, targetName.c_str(), 1 );

			if( targetData != NULL )
			{
				if( OGR_DS_GetLayerCount( targetData ) == 1 )
				{
					targetLayer = OGR_DS_GetLayer( targetData, 0 );
				}
				else
				{
					targetLayer = OGR_DS_GetLayerByName( targetData, sourceLayerName.c_str() );

					if( targetLayer == NULL && OGR_DS_GetLayerCount( targetData ) > 0 )
					{
						targetLayer = OGR_DS_GetLayer( targetData, 0 );
					}
				}

				targetLayerWasExisting = ( targetLayer != NULL );
			}
		}
		else
		{
			error = "file doesn't exist";

			return false;
		}
	}
	else
	{	
		if( stat( targetName.c_str(), &fileInfo ) == 0 )
		{
			if( ! S_ISDIR( fileInfo.st_mode ) && remove( targetName.c_str() ) != 0 )
			{
				error = "unable to delete target data";
				return false;
			}
		}

		targetData = OGR_Dr_CreateDataSource( formatDriver, targetName.c_str(), 0 );		
	}

	if( targetData != NULL )
	{
		if( targetLayer == NULL )
		{
			OGRSpatialReferenceH layerSRS = targetSRS != NULL ? targetSRS : sourceSRS;
			targetLayer = OGR_DS_CreateLayer( targetData, sourceLayerName.c_str(), layerSRS, sourceLayerGeom, NULL );
		}
	}
	else
	{
		error = "unable to create target data";

		return false;
	}

	if( targetLayer == NULL )
	{
		error = "unable to create target layer";
		return false;
	}

	return true;
}

bool Ogr::CloseTarget( void )
{
	if( targetSRS != NULL )
	{
		OSRDestroySpatialReference( targetSRS );
		targetSRS = NULL;
	}

	if( targetData != NULL )
	{
		OGR_DS_Destroy( targetData );
		targetData = NULL;
		targetLayer = NULL;
		targetLayerWasExisting = false;

		targetName.clear();
	}
	else
	{
		return false;
	}

	return true;
}

bool Ogr::Execute( const string &query )
{
	int featuresCount = 0;

	if( Prepare( featuresCount, query ) )
	{
		while( Process() ) ;

		CloseTarget();
		
		CloseSource();
	}
	else
	{
		return false;
	}

	return true;
}

bool Ogr::Prepare( int &featuresCount, const string &query )
{
	error.clear();

	OGRFeatureDefnH featDefn = OGR_L_GetLayerDefn( sourceLayer );
	OGRFeatureDefnH targetDefn = OGR_L_GetLayerDefn( targetLayer );

	for( int i = 0; i < OGR_FD_GetFieldCount( sourceLayerDefn ); i ++ )
	{
		OGRFieldDefnH field = OGR_FD_GetFieldDefn( featDefn, i );

		if( targetLayerWasExisting && OGR_FD_GetFieldIndex( targetDefn, OGR_Fld_GetNameRef( field ) ) >= 0 )
		{
			continue;
		}

		if( Error( OGR_L_CreateField( targetLayer, field, 0 ), error ) )
		{
			return false;
		}
	}

	if( squeryLayer != NULL )
	{
		OGR_DS_ReleaseResultSet( sourceData, squeryLayer );
		squeryLayer = NULL;
		sourceLayer = OGR_DS_GetLayer( sourceData, 0 );
		sourceLayerDefn = OGR_L_GetLayerDefn( sourceLayer );
	}

	if( query.size() > 0 )
	{
		squeryLayer = OGR_DS_ExecuteSQL( sourceData, query.c_str(), NULL, "" );

		if( squeryLayer != NULL )
		{
			sourceLayer = squeryLayer;
			sourceLayerDefn = OGR_L_GetLayerDefn( sourceLayer );
		}
		else
		{
			error = "unable to execute source query";
			return false;
		}
	}

	OGR_L_ResetReading( sourceLayer );

	featuresCount = OGR_L_GetFeatureCount( sourceLayer, 1 );

	return true;
}

bool Ogr::Process( void )
{
	error.clear();

	OGRFeatureH feature;

	if( ( ( feature = OGR_L_GetNextFeature( sourceLayer ) ) != NULL ) )
	{
		if( targetSRS && OGR_F_GetGeometryRef( feature ) != NULL )
		{
			if( Error( OGR_G_TransformTo( OGR_F_GetGeometryRef( feature ), targetSRS ), error ) )
			{
				OGR_F_Destroy( feature );
				return false;
			}
		}

		if( Error( OGR_L_CreateFeature( targetLayer, feature ), error ) )
		{
			OGR_F_Destroy( feature );
			return false;
		}

		OGR_F_Destroy( feature );
	}
	else
	{
		return false;
	}

	return true;
}

const string &Ogr::GetLastError( void ) const
{
	return error;
}

bool Ogr::Error( OGRErr code, string &type )
{
	switch( code )
	{
		case OGRERR_NONE :
		{
			return false;
		}
		break;

		case OGRERR_NOT_ENOUGH_DATA :
		{
			type = "not enough data";
		}
		break;

		case OGRERR_NOT_ENOUGH_MEMORY :
		{
			type = "not enough memory";
		}
		break;

		case OGRERR_UNSUPPORTED_GEOMETRY_TYPE :
		{
			type = "unsupported geometry type";
		}
		break;

		case OGRERR_UNSUPPORTED_OPERATION :
		{
			type = "unsupported operation";
		}
		break;

		case OGRERR_CORRUPT_DATA :
		{
			type = "corrupt data";
		}
		break;

		case OGRERR_FAILURE :
		{
			type = "failure";
		}
		break;

		case OGRERR_UNSUPPORTED_SRS :
		{
			type = "unsupported srs";
		}
		break;

		default :
		{
			type = "unknown";
		}
		break;
	}

	return true;
}
