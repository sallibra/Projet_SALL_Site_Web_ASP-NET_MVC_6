using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Net.Mail;
using System.Net;

namespace AvisFormation.Logic
{
    public class EmailManager
    {
        public void SendEmail(string titre,string message, string email)
        {


                MailMessage msg = new MailMessage();
                msg.From = new MailAddress(email);
                msg.To.Add(new MailAddress("administrateur@avisformation.fr"));
                msg.Subject = "Nouveau message AvisFormation";

                msg.Body = message;
                var client = new SmtpClient
                {
                    Host = "ns0.ovh.net",
                    Port = 587,
                    EnableSsl = false,
                    DeliveryMethod = SmtpDeliveryMethod.Network,
                    UseDefaultCredentials = false,
                    Timeout = 30 * 1000,
                    Credentials = new NetworkCredential("administrateur@avisformation.fr", "Mot de passe")
                };
                client.Send(msg);

        }
    }
}
