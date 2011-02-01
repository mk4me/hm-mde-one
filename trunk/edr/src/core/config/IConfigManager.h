
#pragma once
#ifndef ICONFIGMANAGER_H
#define ICONFIGMANAGER_H

#include <string>

class IConfigManager
{

public:
    /// The character '.' used to separate section names
    const static char kSectionSeperator ='.';

    /// The character '=' used to separate the name from the value in source files and on the
    /// command-line.
    const static char kNameValueSeperator = '=';

    /// The character '>' used to link a name to an existing name/value pair.
    const static char kNameLinkSeperator = '>';

    /// The character '+' used to add an additional file source to the configuration with an
    /// optional section to add it under specified to the left of the separator.
    const static char kNameFileSeperator = '+';

    /// The character '#' used to specify that the name should be converted to a unique name in a
    /// list but replacing the '#' character with unique number in sequence.
    const static char kNameListChar = '#';

    const static char kOpenConfigurationGroupChar = '[';

    const static char kCloseConfigurationGroupChar = ']';

	const static char kSpaceConfigurationGroupChar = ' ';
	
	const static char kSemicolonConfigurationGroupChar = ';';

    /// Characters that start a variable name in a value or source file.
    const static char kVarStart[];

    /// Character that ends a variable name in a value or source file
    const static char kVarEnd[];

    /// Set of characters that are illegal to use in a section name
    const static char kIllegalSectionChars[10];

    /// Set of characters that are illegal to use in a value name.
    const static char kIllegalNameChars[10];


    /**
    Checks if the specified value represents a true value.
    */
    virtual bool IsTrue(const std::string& strName, bool defaultValue = false) const = 0;


    /**
    Checks if the specified value represents a false value.
    */
    virtual bool IsFalse(const std::string& strName, bool defaultValue = true) const = 0;

protected:

};

/// Define a smart pointer (reference counting, self deleting pointer) for the IConfigManager
/// class.
//typedef efd::SmartPointer<IConfigManager> IConfigManagerPtr;


#endif // EE_ICONFIGMANAGER_H
