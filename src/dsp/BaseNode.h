#pragma once

#include <pch.h>

class NodeComponent;
class GraphView;

/**
 * Base class to provide shared functionality for Delay Nodes
 * and Input Nodes
 */
template <typename Child>
class BaseNode
{
public:
    BaseNode();
    virtual ~BaseNode() = default;

    /** Create a GUI editor for this node */
    virtual std::unique_ptr<NodeComponent> createNodeEditor (GraphView*) = 0;

    /** Access the editor for this node, or nullptr if editor does not exist */
    NodeComponent* getEditor();

    virtual void prepare (double sampleRate, int samplesPerBlock);
    virtual void process (AudioBuffer<float>& inBuffer, AudioBuffer<float>& outBuffer);

    virtual float getNodeLevel (float inputLevel) const;

    // Adding/removing children
    Child* addChild();
    void removeChild (Child* childToRemove);
    void clearChildren();

    // Managing children
    int getNumChildren() const noexcept { return children.size(); }
    Child* getChild (int idx) { return children[idx]; }
    BaseNode* getParent() { return parent; }
    virtual void setParent (BaseNode* parent);

    struct Listener
    {
        virtual ~Listener() = default;
        virtual void nodeAdded (Child* /*newNode*/) {}
        virtual void nodeRemoved (Child* /*nodeToRemove*/) {}
        virtual void nodeIndexChanged (Child* /*node*/, int /*oldIndex*/, int /*newIndex*/) {}

        virtual void setParameterDiff (Child* /*node*/, const String& /*paramID*/, float /*diff01*/) {}
        virtual void setParameterDefault (Child* /*node*/, const String& /*paramID*/) {}
        virtual void nodeInsanityLockChanged (Child* /*node*/) {}

        virtual void beginParameterChange (const StringArray& /*paramIDs*/, Child* /*node*/) {}
        virtual void endParameterChange (const StringArray& /*paramIDs*/, Child* /*node*/) {}
        virtual void applyParameterChange (const String& /*paramID*/, Child* /*node*/, float /*value01*/) {}

        virtual void addParameterMenus (PopupMenu& /*parentMenu*/, const String& /*paramID*/, Child* /*node*/) {}

        virtual void saveExtraNodeState (XmlElement* /*nodeState*/, Child* /*node*/) {}
        virtual void loadExtraNodeState (XmlElement* /*nodeState*/, Child* /*node*/) {}
    };

    void addNodeListener (Listener* l) { nodeListeners.add (l); }
    void removeNodeListener (Listener* l) { nodeListeners.remove (l); }

    virtual XmlElement* saveXml();
    virtual void loadXml (XmlElement*);

protected:
    NodeComponent* editor = nullptr;
    BaseNode* parent = nullptr;

    OwnedArray<Child> children;
    ListenerList<Listener> nodeListeners;

private:
    std::unique_ptr<Child> nodeBeingDeleted;

    AudioBuffer<float> childBuffer;

    double sampleRate = 44100.0;
    int samplesPerBlock = 256;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BaseNode)
};
