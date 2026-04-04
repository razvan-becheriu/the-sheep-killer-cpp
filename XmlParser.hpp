#ifndef XMLPARSER_HPP
#define XMLPARSER_HPP

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <stdexcept> // For std::runtime_error
#include <istream>
#include "tinyxml2.h"

// Forward declarations
class TXmlNode;
class TXmlDocument;

class TXmlAttribute {
public:
    std::string key;
    std::string value;
};

class TXmlAttributes {
public:
    std::map<std::string, std::string> attributes;

    std::string getAttribute(const std::string& key) const {
        auto it = attributes.find(key);
        if (it != attributes.end()) {
            return it->second;
        }
        return ""; // Return empty string if not found
    }

    void setAttribute(const std::string& key, const std::string& value) {
        attributes[key] = value;
    }
};

enum TNodeType {
    ndTag,
    ndComment,
    ndText,
    ndCData
};

class TXmlNode {
public:
    TNodeType nodeType;
    std::string name;
    std::string textContent; // For ndText, ndCData, and content of tags
    TXmlAttributes attributes;
    std::vector<std::unique_ptr<TXmlNode>> children;

    // Bridge to TinyXML2
    tinyxml2::XMLElement* internalElement;

    TXmlNode(tinyxml2::XMLElement* element = nullptr);
    ~TXmlNode() = default;

    // Simplified query for direct children
    TXmlNode* query(const std::string& q) const;

    // Helper to get text content as int
    int getNumber() const;

    // Placeholder for loading from stream (full implementation would be complex)
    void load(std::istream& s);

    // Helper to append child with text content
    void appendChildText(const std::string& childName, const std::string& text);
    void appendChildNumber(const std::string& childName, int number);
};

class TXmlDocument {
public:
    tinyxml2::XMLDocument doc;
    std::unique_ptr<TXmlNode> rootNodeWrapper;

    TXmlDocument();
    ~TXmlDocument() = default;

    TXmlNode* query(const std::string& q) const;
    void load(const std::string& filename);
    void load(std::istream& s);
};

#endif // XMLPARSER_HPP