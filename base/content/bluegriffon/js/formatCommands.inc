//-----------------------------------------------------------------------------------

function ApplyToNodesArray(aFunctor, aEditor)
{
  var nodes = [], node;
  if (aEditor
      && aEditor.selection.rangeCount > 1) {
    var selection = aEditor.selection;
    var count = selection.rangeCount;
    for (var i = 0; i < count; i++) {
      var range = selection.getRangeAt(i);
      if (range.startContainer == range.endContainer
          && range.startOffset + 1 == range.endOffset
          && range.startContainer.nodeType == Node.ELEMENT_NODE) {
        node = range.startContainer.childNodes[range.startOffset];
      }
      else
        node = selection.getRangeAt(i).commonAncestorContainer;
      nodes.push(node);
    }
  }
  else
    nodes.push(EditorUtils.getSelectionContainer().node);

  aEditor.beginTransaction();
  for (var k = 0 ; k < nodes.length; k++) {
    node = nodes[k];
    aFunctor(node, arguments);
  }
  aEditor.endTransaction();

  return nodes;
}

var cmdClass =
{
  isCommandEnabled: function(aCommand, dummy)
  {
    return (EditorUtils.getCurrentEditorElement() &&
            EditorUtils.isDocumentEditable() &&
            EditorUtils.isEditingRenderedHTML() &&
            GetCurrentViewMode() == "wysiwyg");
  },

  getCommandStateParams: function (aCommandName, aParams, aRefcon)
  {
    var enabled = this.isCommandEnabled(aCommandName, aRefcon);
    aParams.setBooleanValue("state_enabled", enabled);
    aParams.setBooleanValue("state_mixed", false);
    var selContainer = EditorUtils.getSelectionContainer();
    if (selContainer && selContainer.node)
    {
      var classes = selContainer.node.className;
      if (classes)
      {
        aParams.setCStringValue("state_attribute", classes);
        return;
      }
    }

    aParams.setCStringValue("state_attribute", "");
  },

  doCommandParams: function(aCommandName, aParams, aRefCon)
  {
    var className = aParams.getCStringValue("state_attribute");

    function functor(aNode) {
      var editor = arguments[1][1];
      var cn = arguments[1][2];

      if (cn) {
        var classes = aNode.classList;
    
        var newList = cn;
        if (classes)
        {
          var found = false;
          newList = [];
          for (var i = 0; i < classes.length; i++)
          {
            if (classes[i] == cn)
              found = true;
            else
            newList.push(classes[i]);;
          }
          if (!found)
            newList.push(cn);
        }
        if (newList.length)
          editor.setAttribute(aNode, "class", newList.join(" "));
        else
          editor.removeAttribute(aNode, "class");
      }
      else
        editor.removeAttribute(aNode, "class");
    }

    var nodes = ApplyToNodesArray(functor,
                                  EditorUtils.getCurrentEditor(),
                                  className);

    // be kind with the rest of the world
    NotifierUtils.notify("selection", nodes[0], false);
  }
};

var cmdId =
{
  isCommandEnabled: function(aCommand, dummy)
  {
    return (EditorUtils.getCurrentEditorElement() &&
            EditorUtils.isDocumentEditable() &&
            EditorUtils.isEditingRenderedHTML() &&
            GetCurrentViewMode() == "wysiwyg");
  },

  getCommandStateParams: function (aCommandName, aParams, aRefcon)
  {
    var enabled = this.isCommandEnabled(aCommandName, aRefcon);
    aParams.setBooleanValue("state_enabled", enabled);
    aParams.setBooleanValue("state_mixed", false);
    var selContainer = EditorUtils.getSelectionContainer();
    if (selContainer && selContainer.node)
    {
      var id = selContainer.node.id;
      if (id)
      {
        aParams.setCStringValue("state_attribute", id);
        return;
      }
    }

    aParams.setCStringValue("state_attribute", "");
  },

  doCommandParams: function(aCommandName, aParams, aRefCon)
  {
    var id = aParams.getCStringValue("state_attribute");
    var node = EditorUtils.getSelectionContainer().node;
    var editor = EditorUtils.getCurrentEditor();
    var elt = id ? EditorUtils.getCurrentDocument().getElementById(id) : null;
    var rv = 0;
    if (elt && elt != node)
      rv = PromptUtils.confirmWithTitle(
                             L10NUtils.getString("IdAlreadyTaken"),
                             L10NUtils.getString("RemoveIdFromElement"),
                             L10NUtils.getString("YesRemoveId"),
                             L10NUtils.getString("NoCancel"),
                             null);

    if (id && node.id != id) {
      // first, let's check another element does not already carry that id...
      if (rv == 1)
        return;
      editor.beginTransaction();
      if (elt)
        editor.removeAttribute(elt, "id");
      editor.setAttribute(node, "id", id);
      editor.endTransaction();
    }
    else
      editor.removeAttribute(node, "id");

    // be kind with the rest of the world
    NotifierUtils.notify("selection", node, false);
  }
};

//-----------------------------------------------------------------------------------

var cmdAriaRole =
{
  isCommandEnabled: function(aCommand, dummy)
  {
    return (EditorUtils.getCurrentEditorElement() &&
            EditorUtils.isDocumentEditable() &&
            EditorUtils.isEditingRenderedHTML() &&
            GetCurrentViewMode() == "wysiwyg");
  },

  getCommandStateParams: function (aCommandName, aParams, aRefcon)
  {
    var enabled = this.isCommandEnabled(aCommandName, aRefcon);
    aParams.setBooleanValue("state_enabled", enabled);
    aParams.setBooleanValue("state_mixed", false);
    var selContainer = EditorUtils.getSelectionContainer();
    if (selContainer)
    {
      var role = selContainer.node.getAttribute("role");
      if (role)
      {
        aParams.setCStringValue("state_attribute", role);
        return;
      }
    }

    aParams.setCStringValue("state_attribute", "");
  },

  doCommandParams: function(aCommandName, aParams, aRefCon)
  {
    var role = aParams.getCStringValue("state_attribute");

    function functor(aNode) {
      var editor = arguments[1][1];
      var r = arguments[1][2];
      if (r)
        editor.setAttribute(aNode, "role", r);
      else
        editor.removeAttribute(aNode, "role");
    }

    var nodes = ApplyToNodesArray(functor,
                                  EditorUtils.getCurrentEditor(),
                                  role);

    // be kind with the rest of the world
    NotifierUtils.notify("selection", nodes[0], false);

  }
};

var cmdPagePropertiesCommand =
{
  isCommandEnabled: function(aCommand, dummy)
  {
    return (EditorUtils.getCurrentEditorElement() &&
            EditorUtils.isDocumentEditable() &&
            EditorUtils.isEditingRenderedHTML() &&
            GetCurrentViewMode() == "wysiwyg");
  },

  getCommandStateParams: function(aCommand, aParams, aRefCon) {},
  doCommandParams: function(aCommand, aParams, aRefCon) {},

  doCommand: function(aCommand)
  {
    try {
      window.openDialog("chrome://bluegriffon/content/dialogs/pageProperties.xul", "_blank",
              "chrome,close,titlebar,modal");
    }
    catch(ex) {}
    window.content.focus();
  }
};

var cmdEditListCommand =
{
  isCommandEnabled: function(aCommand, dummy)
  {
    var element = EditorUtils.getObjectForProperties(["ul","ol"]);
    return (element &&
            EditorUtils.getCurrentEditorElement() &&
            EditorUtils.isDocumentEditable() &&
            EditorUtils.isEditingRenderedHTML() &&
            GetCurrentViewMode() == "wysiwyg");
  },

  getCommandStateParams: function(aCommand, aParams, aRefCon) {},
  doCommandParams: function(aCommand, aParams, aRefCon) {},

  doCommand: function(aCommand)
  {
    window.openDialog("chrome://bluegriffon/content/dialogs/listProperties.xul","_blank",
                      "chrome,modal,titlebar,resizable=no,dialog=yes");
   }
};
