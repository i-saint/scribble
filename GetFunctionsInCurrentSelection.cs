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

        private static void GetFunctionsInCurrentSelectionImpl(TextSelection selection, CodeElements elements, List<String> functions)
        {
            for (int i = 1; i <= elements.Count; ++i)
            {
                CodeElement element = elements.Item(i);
                if (element.Kind == vsCMElement.vsCMElementFunction)
                {
                    if (
                        (element.StartPoint.Line >= selection.TopPoint.Line && element.StartPoint.Line <= selection.BottomPoint.Line) ||
                        (element.EndPoint.Line   >= selection.TopPoint.Line && element.EndPoint.Line <= selection.BottomPoint.Line) ||
                        (element.StartPoint.Line <= selection.TopPoint.Line && element.EndPoint.Line >= selection.BottomPoint.Line)
                        )
                    {
                        functions.Add(element.FullName);
                    }
                }
                else
                {
                    GetFunctionsInCurrentSelectionImpl(selection, element.Children, functions);
                }
            }
        }

        public static List<String> GetFunctionsInCurrentSelection(DTE dte)
        {
            List<String> ret = new List<String>();
            if (dte.ActiveDocument != null) {
                FileCodeModel filecm = dte.ActiveDocument.ProjectItem.FileCodeModel;
                if (filecm != null) {
                    TextDocument tdoc = dte.ActiveDocument.Object() as TextDocument;
                    GetFunctionsInCurrentSelectionImpl(tdoc.Selection, filecm.CodeElements, ret);
                }
            }
            return ret;
        }
    }
}
