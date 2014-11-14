using UnityEngine;
using System.Collections;

public class ApplicationManager : MonoBehaviour {
	public Material hex;
	public float fader = 0.0f;

	public void Quit () 
	{
		#if UNITY_EDITOR
		UnityEditor.EditorApplication.isPlaying = false;
		#else
		Application.Quit();
		#endif
	}
	
	void Update()
	{
		hex.SetFloat("_PatternFade", fader);
	}
}
