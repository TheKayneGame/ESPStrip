using System;
using System.Collections.Generic;
using System.Data;
using System.Linq;
using System.Net;
using System.Net.Sockets;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;

namespace WinESPStrip
{
	/// <summary>
	/// Interaction logic for MainWindow.xaml
	/// </summary>
	public partial class MainWindow : Window
	{
		static int Port = 53686;
		string ESPStripHost = "ESPStrip.local";
		string EspIP;
		UdpClient Client = new UdpClient(Port);

		int r;
		int g;
		int b;
		int s;
		int e;
		int brg;
		int a;
		int arg;


		private void onLoad(object sender, RoutedEventArgs e)
		{
			ConnectToESP();
		}

		private void SendUDPToLED(string command)
		{
			if (EspIP != null)
			{
				Socket sock = new Socket(AddressFamily.InterNetwork, SocketType.Dgram, ProtocolType.Udp);

				IPAddress serverAddr = IPAddress.Parse(EspIP);

				IPEndPoint endPoint = new IPEndPoint(serverAddr, Port);
				byte[] send_buffer = Encoding.ASCII.GetBytes(command);
				sock.SendTo(send_buffer, endPoint);
			}
			else
			{
				MessageBox.Show("Test");
			}
		}

		private void ConnectToESP()
		{
			Log("Connecting to ESPStrip");
			Task.Run(() =>
			{
				try
				{
					IPAddress[] IPlist = Dns.GetHostAddresses(ESPStripHost);
					foreach (IPAddress IP in IPlist)
					{
						Console.WriteLine(IP.ToString());
						EspIP = IP.ToString();
						Dispatcher.Invoke(() => {
							Log("Connected to ESPStrip");
						});
						
					}
				}
				catch (SocketException ex)
				{
					Console.WriteLine(ex.ToString());					
					Dispatcher.Invoke(() => {
						Log("Could not connect to ESPStrip");
					});
				}
			});
			
		}

		private string CreateESPCommand(int r, int g, int b, int s, int e, int brg, int a, int arg)
		{
			string Command = r + ":" + g + ":" + b + ":" + s + ":" + e + ":" + brg + ":" + a + ":" + arg + ":";
			return Command;
		}

		private void Log(string i)
		{
			Logs.Items.Add(i);
		}

		private void button_Click(object sender, RoutedEventArgs e)
		{
			SendUDPToLED(CreateESPCommand(255, 0, 0, 1, 60, 20, 1, 1));
		}

		private void IPTemp_TextChanged(object sender, TextChangedEventArgs e)
		{

		}

		private void ClrPcker_Background_SelectedColorChanged(object sender, RoutedPropertyChangedEventArgs<Color?> e)
		{
			r = Color.SelectedColor.Value.R;
			g = Color.SelectedColor.Value.G;
			b = Color.SelectedColor.Value.B;
			SendUDPToLED(CreateESPCommand(r, g, b, 1, 60, 20, 0, 0));

			Log(Color.SelectedColor.Value.R.ToString() + " " + Color.SelectedColor.Value.G.ToString() + " " +Color.SelectedColor.Value.B.ToString());
		}
	}
}
