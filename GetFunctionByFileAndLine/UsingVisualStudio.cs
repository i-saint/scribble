using System;
using System.IO;
using System.Reflection;
using System.Text.RegularExpressions;
using System.Collections.Generic;
using EnvDTE;
using Microsoft.VisualStudio.VCCodeModel;


namespace primitive
{
    public class ProjectUtil
    {
        private static void GetFunctionInCurrentPositionImpl(CodeElement element, List<String> functions)
        {
            if (element == null) { return; }
            if (element.Kind == vsCMElement.vsCMElementFunction)
            {
                functions.Add(element.FullName);
            }
            else if (
                element.Kind == vsCMElement.vsCMElementNamespace ||
                element.Kind == vsCMElement.vsCMElementClass ||
                element.Kind == vsCMElement.vsCMElementStruct ||
                element.Kind == vsCMElement.vsCMElementUnion
                )
            {
                foreach (CodeElement child in element.Children)
                {
                    GetFunctionInCurrentPositionImpl(child, functions);
                }
            }
        }

        public static List<String> GetFunctionInCurrentPosition(DTE dte)
        {
            List<String> ret = new List<String>();
            if (dte.ActiveDocument != null)
            {
                try
                {
                    TextDocument tdoc = dte.ActiveDocument.Object() as TextDocument;
                    GetFunctionInCurrentPositionImpl(tdoc.Selection.ActivePoint.get_CodeElement(vsCMElement.vsCMElementNamespace), ret);
                }
                catch (Exception)
                {
                }
            }
            return ret;
        }
    }
}
