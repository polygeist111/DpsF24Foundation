using System;
using System.Net;
using System.Net.Sockets;
using System.Text;
using UnityEngine;

public class DataIO : MonoBehaviour
{
    public GameObject objectToTrack; // Object to track and send position data
    public int flashlightButtonState;
    public int potentiometerValue;

    UdpClient udpClient;
    IPEndPoint remoteEndPoint;

    void Start()
    {
        // Set up UDP to receive data from Arduino
        udpClient = new UdpClient(4211);
        remoteEndPoint = new IPEndPoint(IPAddress.Any, 0);
        udpClient.BeginReceive(new AsyncCallback(ReceiveCallback), null);
        
        // Set up UDP to send data to Arduino (replace with the Arduino's IP)
        remoteEndPoint = new IPEndPoint(IPAddress.Parse("192.168.1.132"), 4211); // Adjust IP and port if necessary
    }

    void Update()
    {
        // Get the position of the tracked object
        Vector3 position = objectToTrack.transform.position;

        // Format the position data as a string
        string positionData = $"X:{position.x},Y:{position.y},Z:{position.z}";

        // Convert the string to bytes
        byte[] data = Encoding.ASCII.GetBytes(positionData);

        // Send the UDP packet to the Arduino
        udpClient.Send(data, data.Length, remoteEndPoint);

        Debug.Log("Sent Position: " + positionData);
    }

    void ReceiveCallback(IAsyncResult ar)
    {
        // Receive the UDP packet from Arduino
        byte[] receivedBytes = udpClient.EndReceive(ar, ref remoteEndPoint);
        string receivedText = Encoding.ASCII.GetString(receivedBytes);
        Debug.Log("Received: " + receivedText);

        // Parse the received data (flashlight button and potentiometer values)
        string[] data = receivedText.Split(',');
        if (data.Length == 2)
        {
            int.TryParse(data[0].Split(':')[1].Trim(), out flashlightButtonState);
            int.TryParse(data[1].Split(':')[1].Trim(), out potentiometerValue);
        }

        // Continue receiving data
        udpClient.BeginReceive(new AsyncCallback(ReceiveCallback), null);
    }

    void OnApplicationQuit()
    {
        // Close the UDP client on quit
        udpClient.Close();
    }
}
