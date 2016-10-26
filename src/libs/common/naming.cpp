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

#include "naming.h"
#include <QRegularExpression>
#include <QDir>
#include <QFileInfo>

// +-----------------------------------------------------------
fsdk::Naming::Naming()
{
}

// +-----------------------------------------------------------
fsdk::Naming::WildcardListingReturn fsdk::Naming::wildcardListing(const QString &sSourceWildcard, const QString &sTargetWildcard, QMap<QString, QString> &mMatchedListing)
{
	// Regular expression that finds the a wildcard
	// (with only one wildcard character, or with all the characters between 
	// the first and the last wildcard characters)
	QRegularExpression oWildcardRE("[\\?\\*\\[\\]].*[\\?\\*\\[\\]]|[\\?\\*\\[\\]]");

	// Check if the source wildcard has a wildcard in the proper place
	QFileInfo oSource(sSourceWildcard);
	QDir oSourceDir = oSource.dir();
	QString sSourceFilePart = oSource.fileName();

	if(oSourceDir.absolutePath().contains(oWildcardRE))
		return InvalidSourceWildcard;

	// Check if the target wildcard has a wildcard in the proper place
	QFileInfo oTarget(sTargetWildcard);
	QDir oTargetDir = oTarget.dir();
	QString sTargetFilePart = oTarget.fileName();

	if(oTargetDir.absolutePath().contains(oWildcardRE))
		return InvalidTargetWildcard;

	// Check if the wildcards are the same in both arguments
	QRegularExpressionMatch oSourceMatch = oWildcardRE.match(sSourceFilePart);
	QRegularExpressionMatch oTargetMatch = oWildcardRE.match(sTargetFilePart);

	if(oSourceMatch.captured(0) != oTargetMatch.captured(0))
		return DifferentWildcards;

	// If no wildcard has indeed been provided, just replicate the target
	// in the final listing and return ok
	if(oSourceMatch.captured(0).isEmpty())
	{
		if(!QFile::exists(sSourceWildcard))
			return FileNotExist;

		mMatchedListing[sSourceWildcard] = sTargetWildcard;
		return ListingOk;
	}

	// Otherwise, filter all source files with the given source wildcard
	oSourceDir.setNameFilters(QStringList(sSourceFilePart));
	if(oSourceDir.entryInfoList().count() == 0)
		return EmptySourceListing;

	// Process the mapping to the target names according to the parts under
	// the wildcard in the source names
	QString sSourcePrefix = sSourceFilePart.mid(0, oSourceMatch.capturedStart(0));
	QString sSourcePostfix = sSourceFilePart.mid(oSourceMatch.capturedEnd(0));
	QString sTargetPrefix = sTargetFilePart.mid(0, oTargetMatch.capturedStart(0));
	QString sTargetPostfix = sTargetFilePart.mid(oTargetMatch.capturedEnd(0));

	mMatchedListing.clear();
	QString sMiddle, sSourceFilename, sTargetFilename;
	int iPos, iLen;
	foreach(QFileInfo oFile, oSourceDir.entryInfoList())
	{
		// Get the "real" text in the place of the wildcard in the source file name 
		iPos = oFile.fileName().indexOf(sSourcePrefix) + sSourcePrefix.length();
		iLen = oFile.fileName().indexOf(sSourcePostfix) - sSourcePrefix.length();
		sMiddle = oFile.fileName().mid(iPos, iLen);

		// Create a target name with that middle text in place of the target wildcard
		sSourceFilename = QString("%1/%2").arg(oSourceDir.absolutePath(), oFile.fileName());
		sTargetFilename = QString("%1/%2%3%4").arg(oTargetDir.absolutePath(), sTargetPrefix, sMiddle, sTargetPostfix);

		// Add to the mapping
		mMatchedListing[sSourceFilename] = sTargetFilename;
	}

	return ListingOk;
}

