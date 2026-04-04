#include "XmlParser.hpp"
#include <iostream>
#include <sstream>
#include <algorithm> // For std::transform
#include <fstream>
#include <iterator>

// --- TXmlNode Implementation ---

TXmlNode::TXmlNode(tinyxml2::XMLElement* element)
    : internalElement(element) {
    if (element) {
        name = element->Name();
        const char* text = element->GetText();
        textContent = text ? text : "";
        
        // Map attributes
        const tinyxml2::XMLAttribute* attr = element->FirstAttribute();
        while (attr) {
            attributes.setAttribute(attr->Name(), attr->Value());
            attr = attr->Next();
        }

        // Recursively populate children so that the model hierarchy and animations can be iterated
        for (tinyxml2::XMLElement* child = element->FirstChildElement(); child; child = child->NextSiblingElement()) {
            children.push_back(std::make_unique<TXmlNode>(child));
        }
    }
}

TXmlNode* TXmlNode::query(const std::string& q) const {
    for (const auto& child : children) {
        if (child->name == q) {
            return child.get();
        }
    }
    return nullptr;
}

int TXmlNode::getNumber() const {
    return std::stoi(textContent.empty() ? "0" : textContent);
}

void TXmlNode::load(std::istream& s) {
    // This is handled by TXmlDocument::load
}

void TXmlNode::appendChildText(const std::string& childName, const std::string& text) {
    if (!internalElement) return;
    tinyxml2::XMLDocument* doc = internalElement->GetDocument();
    tinyxml2::XMLElement* newElem = doc->NewElement(childName.c_str());
    newElem->SetText(text.c_str());
    internalElement->InsertEndChild(newElem);
}

void TXmlNode::appendChildNumber(const std::string& childName, int number) {
    appendChildText(childName, std::to_string(number));
}

// --- TXmlDocument Implementation ---

TXmlDocument::TXmlDocument() : rootNodeWrapper(nullptr) {}

TXmlNode* TXmlDocument::query(const std::string& q) const {
    if (rootNodeWrapper && rootNodeWrapper->name == q) {
        return rootNodeWrapper.get();
    }
    return nullptr;
}

void TXmlDocument::load(const std::string& filename) {
    if (doc.LoadFile(filename.c_str()) != tinyxml2::XML_SUCCESS) {
        throw std::runtime_error("TinyXML2: Failed to load " + filename);
    }
    rootNodeWrapper = std::make_unique<TXmlNode>(doc.RootElement());
}

void TXmlDocument::load(std::istream& s) {
    std::string content((std::istreambuf_iterator<char>(s)), std::istreambuf_iterator<char>());
    if (doc.Parse(content.c_str()) != tinyxml2::XML_SUCCESS) {
        throw std::runtime_error("TinyXML2: Failed to parse XML stream");
    }
    rootNodeWrapper = std::make_unique<TXmlNode>(doc.RootElement());
}