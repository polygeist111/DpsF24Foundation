//https://www.sharpcoderblog.com/blog/flashlight-tutorial-for-unity
// new comment added

using UnityEngine;

public class FlashlightController : MonoBehaviour
{
    // Public variables
    public AudioClip turnOnSound;
    public AudioClip turnOffSound;

    public int potVal;
    

    // Private variables
    private Light flashlight;
    private AudioSource audioSource;
    private bool hasSwitched = false;

    private void Start()
    {
        // Get Light component in the same GameObject
        flashlight = GetComponent<Light>();

        if (flashlight == null)
        {
            Debug.LogWarning("Light component is not attached. Attach a Light component manually.");
        }
        else
        {
            flashlight.enabled = false;
        }

        // Get or add AudioSource component to the same GameObject!
        audioSource = GetComponent<AudioSource>();
        if (audioSource == null)
        {
            audioSource = gameObject.AddComponent<AudioSource>();
            audioSource.playOnAwake = false;
        }
    }

    private void Update()
    {
        potVal = GameObject.Find("DataIO").GetComponent<DataIO>().potentiometerValue;
        flashlight.intensity = potVal * .01f;

        //float distanceBetweenObjects = GameObject.Find("Sphere").GetComponent<DistanceFromObject01>().dist;
        int switchVal26 = GameObject.Find("DataIO").GetComponent<DataIO>().flashlightButtonState;
        if(switchVal26 == 0 && hasSwitched == false)
        //if (Input.GetKeyDown(KeyCode.F))

        

        {
            hasSwitched = true;
            if (flashlight != null)
            {
                flashlight.enabled = !flashlight.enabled;

                

                // Play audio effect based on flashlight state
                if (flashlight.enabled)
                {
                    PlayAudioEffect(turnOnSound);
                }
                else
                {
                    PlayAudioEffect(turnOffSound);
                }
            }
            else
            {
                Debug.LogWarning("Cannot control flashlight as Light component is not attached.");
            }
        }
        if(switchVal26 == 1) {
            hasSwitched = false;
        }
    }

    private void PlayAudioEffect(AudioClip clip)
    {
        if (clip != null)
        {
            audioSource.clip = clip;
            audioSource.Play();
        }
    }
}
