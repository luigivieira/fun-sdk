/*
 * Copyright (C) 2016-2017 Luiz Carlos Vieira (http://www.luiz.vieira.nom.br)
 *
 * This file is part of Fun SDK (FSDK).
 *
 * FSDK is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * FSDK is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef NAMING_H
#define NAMING_H

#include "libexport.h"
#include <QMap>

namespace fsdk
{
	/**
	 * Useful naming functions for files and directories.
	 */
	class SHARED_LIB_EXPORT Naming
	{
	protected:
		/**
		 * Protected constructor, so the class can not be instantiated.
		 */
		Naming();

	public:
		
		enum WildcardListingReturn
		{
			/** The source wildcard is invalid. */
			InvalidSourceWildcard,

			/** The target wildcard is invalid. */
			InvalidTargetWildcard,

			/** The source and target wildcards are not the same. */
			DifferentWildcards,

			/** The source wildcard did not result in any existing file. */
			EmptySourceListing,

			/** The method concluded ok and the listing was produced. */
			ListingOk
		};

		/**
		 * List existing files according to a given source wildcard
		 * and produce new names according to the given target wildcard.
		 * The wildcards allowed follow the rules of QRegExp wildcard matching
		 * (http://doc.qt.io/qt-5/qregexp.html#wildcard-matching):
		 * 
		 *     'c' Any character represents itself apart from those
		 *         mentioned below. Thus c matches the character 'c'.
         *     '?' Matches any single character. It is the same as '.' in
		 *         full regexps.
         *     '*' Matches zero or more of any characters. It is the same
		 *         as '.*' in full regexps.
         *     '[...]' Sets of characters can be represented in square brackets,
		 *       similar to full regexps. Within the character class, like
		 *       outside, backslash has no special meaning.
		 *
		 * @param sSourceWildcard Path and name of the source file, with wildcards
		 * only in the filename part. This mask will be used to list the existing
		 * files in disk and collected their names.
		 * @param sTargetWildcard Path and name of the target file, with wildcards
		 * only in the filename part. This mask will be used to create the new names
		 * based on the wildcard mask and the rest. The wildcard mask in this argument
		 * must match exactly the wildcard mask in the sSourceWildcard argument. All
		 * other characters can be anything.
		 * @param mMatchedListing QMap<QString, QString> that maps the names of the
		 * files listed based on the sSourceWildcard to the names of the files that
		 * can be created based on the sTargetWildcard.
		 * @return 
		 */
		static WildcardListingReturn wildcardListing(const QString &sSourceWildcard, const QString &sTargetWildcard, QMap<QString, QString> &mMatchedListing);


	};
}

#endif // NAMING_H